import schedule
import time
import subprocess
from ctypes import *
import tensorflow as tf
import numpy as np

from lf_quant import lf_quant
from lf_generate_snapshot import lf_generate_snapshot

import template

kernel_model_quant_path = "../build/"
represetative_dataset = "../data/aurora/representative_dataset.npy"
bash_command_for_compile = "cd ../build; make"
fidelity_loss_threshold = 0

appid_for_cc = 1
uuid = 0
current_installed_model = ""

def _quant_model_output_path(uuid):
    return  "../build/aurora_int_quan_model_{}.tflite".format(uuid)

def _bash_command_for_install_nn(uuid):
    return "cd ../build; insmod lf_model_{}.ko".format(uuid)

def _tf_inference(path):
    predictions = []
    _dataset = np.load(represetative_dataset).astype(np.float32)
    dataset = tf.data.Dataset.from_tensor_slices(_dataset).batch(1)

    interpreter = tf.lite.Interpreter(model_path=path)
    interpreter.allocate_tensors()
    input_index = interpreter.get_input_details()[0]["index"]
    output_index = interpreter.get_output_details()[0]["index"]

    for _data in _dataset:
        test_data = np.expand_dims(_data, axis=0).astype(np.float32)

        interpreter.set_tensor(input_index, test_data)
        interpreter.invoke()
        prediction = interpreter.get_tensor(output_index)
        predictions.append(prediction)
    return predictions

def _cal_fidelity_loss(prediction1, prediction2):
    deltas = np.array(prediction1) - np.array(prediction2)
    agg_delta = 0
    for _delta in deltas:
        agg_delta += abs(_delta)
    return agg_delta/len(prediction1)

def _quant_nn_from_saved_model(path):
    quant_model_output_path_real = _quant_model_output_path(uuid)
    lf_quant(path, represetative_dataset, quant_model_output_path_real)
    lf_generate_snapshot(quant_model_output_path_real, uuid, appid_for_cc, kernel_model_quant_path, False)
    TEMPLATE_FILE = "Makefile.jinja"
    _template = template.get_template(TEMPLATE_FILE)
    code = _template.render(uuid=uuid)
    OUTPUT_FILE = f"{kernel_model_quant_path}Makefile"
    with open(OUTPUT_FILE, "w") as output_file:
        output_file.write(code)
    return quant_model_output_path_real

def _install_nn(quant_model_output_path_real):
    subprocess.run(bash_command_for_compile, shell=True)
    subprocess.run(_bash_command_for_install_nn(uuid), shell=True)

def nn_freeze():
    # The function should freeze the model and 
    # return the path to the saved model
    
    #Returns:
    #    path:The path to the saved model

    return "../data/aurora/"

def nn_evaluate():
    # The function is used to return the stability value
    # of a given model and it's output when feeding the 
    # represetative dataset

    #Returns:
    #    quant_model_path: The path of the quantized model
    #    stability: The stability of the model
    #    fidelity loss: The fidelity loss of the model

    # Here we use a emulated way to obtain a series of 
    # Aurora model to elimate the randomness during online
    # adaptation

    new_freezed_nn_path = nn_freeze()
    quant_model_output_path_real=_quant_nn_from_saved_model(new_freezed_nn_path)
    prediction1 = _tf_inference(quant_model_output_path_real)
    prediction2 = _tf_inference(current_installed_model)
    fedility_loss = _cal_fidelity_loss(prediction1, prediction2)

    return quant_model_output_path_real, True, fedility_loss

def nn_adapt():
    # The function should be implemented to adapt
    # the model

    # The implementation of this function is out of
    # the scope of LiteFlow, thus we leave it empty

    return

def _liteflow_step():
    global uuid, current_installed_model
    nn_adapt()
    quant_model_output_path_real, stability, fidelity_loss = nn_evaluate()
    if stability == True and fidelity_loss >= fidelity_loss_threshold:
        _install_nn(quant_model_output_path_real)
        uuid = uuid + 1
        current_installed_model = quant_model_output_path_real

def liteflow():
    print("LiteFlow userspace start!")
    print("ATTENTION: It is for DEMO purpose...")

    global uuid, current_installed_model
    freezed_nn_path = nn_freeze();
    quant_model_output_path_real=_quant_nn_from_saved_model(freezed_nn_path)
    _install_nn(quant_model_output_path_real)
    uuid = uuid + 1
    current_installed_model = quant_model_output_path_real

    schedule.every(1).minutes.do(_liteflow_step)

    while True:
        schedule.run_pending()
        time.sleep(1)
    return

if __name__ == "__main__":
    liteflow()