# The tflite file can be further converted into json using
# flatc -t --strict-json --defaults-json  third_party/tensorflow/tensorflow/lite/schema/schema.fbs -- nn-loader/sample/aurora/converted_int_quan_model.tflite 

import click
import tensorflow as tf
import numpy as np

@click.command()
@click.argument('saved_model_dir', type=click.Path(exists=True, file_okay=False, dir_okay=True))
@click.argument('dataset_path', type=click.Path(exists=True, file_okay= True, dir_okay=False))
@click.argument('export_path', type=click.Path(exists=False, file_okay=True, dir_okay=False, writable=True))
def lf_quant_cmd(saved_model_dir, dataset_path, export_path):
    return lf_quant(saved_model_dir, dataset_path, export_path)

def lf_quant(saved_model_dir, dataset_path, export_path):

    _dataset = np.load(dataset_path).astype(np.float32)
    dataset = tf.data.Dataset.from_tensor_slices(_dataset).batch(1)

    def representative_data_gen():
        for input_value in dataset.take(60):
            yield [input_value]

    converter = tf.lite.TFLiteConverter.from_saved_model(saved_model_dir)
    converter.optimizations = [tf.lite.Optimize.OPTIMIZE_FOR_LATENCY]
    converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
    converter.inference_input_type = tf.uint8
    converter.inference_output_type = tf.uint8
    converter.representative_dataset = representative_data_gen

    tflite_model = converter.convert()
    open(export_path, "wb").write(tflite_model)

    click.echo("Convert done ...")

    click.echo("Begin inspecting model")

    interpreter = tf.lite.Interpreter(model_path=export_path)
    interpreter.allocate_tensors()
    input_index_quant = interpreter.get_input_details()[0]["index"]
    output_index_quant = interpreter.get_output_details()[0]["index"]

    click.echo("Input index quant: {}, output index quant: {}".format(input_index_quant, output_index_quant))
    
    for _data in _dataset:
        test_data = np.expand_dims(_data, axis=0).astype(np.float32)
        click.echo("Test input: %s" % test_data)

        interpreter.set_tensor(input_index_quant, test_data)
        interpreter.invoke()
        predictions = interpreter.get_tensor(output_index_quant)

        click.echo("Output: %s" % predictions)


if __name__ == "__main__":
    lf_quant_cmd()