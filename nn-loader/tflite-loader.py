import click
import tflite
import numpy as np

from layer import *


# TODO, Add output layer name to manually stop at some layer
@click.command()
@click.argument('path', type=click.Path(exists=True, file_okay=True, dir_okay=False))
def tflite_loader(path):
    click.echo("Reading model from %s ..." % click.format_filename(path))
    if not path.endswith('.tflite'):
        click.echo("The file should end with .tflite")
        return

    with open(path, 'rb') as f:
        buf = f.read()
        model = tflite.Model.GetRootAsModel(buf, 0)

    graph = model.Subgraphs(0)
    num_ops = graph.OperatorsLength()
    layer_list = []
    for op_index in range(0, num_ops - 1):
        op = graph.Operators(op_index)
        op_code = model.OperatorCodes(op.OpcodeIndex())
        if op_code.BuiltinCode() == tflite.BuiltinOperator.FULLY_CONNECTED:
            assert(op.InputsLength() == 3)
            input_tensor, input_buffer = get_tensor_and_buffer(model, graph, op.Inputs(0))
            weight_tensor, weight_buffer = get_tensor_and_buffer(model, graph, op.Inputs(1))
            bias_tensor, bias_buffer = get_tensor_and_buffer(model, graph, op.Inputs(2))

            layer = FCLayer(op_code, input_tensor, weight_tensor, bias_tensor,
                                        input_buffer, weight_buffer, bias_buffer)

        elif op_code.BuiltinCode() == tflite.BuiltinOperator.TANH:
            assert(op.InputsLength() == 1)
            assert(op.OutputsLength() == 1)
            input_tensor, input_buffer = get_tensor_and_buffer(model, graph, op.Inputs(0))
            output_tensor, output_buffer = get_tensor_and_buffer(model, graph, op.Outputs(0))

            layer = TanhLayer(op_code, input_tensor, output_tensor, input_buffer, output_buffer)
        
        elif op_code.BuiltinCode() == tflite.BuiltinOperator.QUANTIZE:
            assert(op.InputsLength() == 1)
            assert(op.OutputsLength() == 1)
            input_tensor, input_buffer = get_tensor_and_buffer(model, graph, op.Inputs(0))
            output_tensor, output_buffer = get_tensor_and_buffer(model, graph, op.Outputs(0))

            layer = QuanLayer(op_code, input_tensor, output_tensor, input_buffer, output_buffer)
            
        else:
            click.echo("Unsupported OP Code: %s ..." % op_code.BuiltinCode())
            continue
        layer_list.append(layer)
    
    generate_c_code(layer_list)

def get_tensor_and_buffer(model, graph, input):
    tensor = graph.Tensors(input)
    tensor_type = tensor.Type()
    raw_buffer = model.Buffers(tensor.Buffer()).DataAsNumpy()

    viewer = None
    if tensor_type == tflite.TensorType.FLOAT32:
        type = np.float32 
    elif tensor_type == tflite.TensorType.INT8:
        type = np.int8 
    elif tensor_type == tflite.TensorType.INT32:
        type = np.int32 
    else:
        raise Exception('Unsupported Tensor Type: %s ...' % tensor_type)

    if isinstance(raw_buffer, np.ndarray):
        buffer = raw_buffer.astype(type)
    else:
        buffer = None

    return tensor, buffer

def generate_c_code(layer_list):
    # TODO
    pass

if __name__ == "__main__":
    tflite_loader()
