import click
import tflite
import numpy as np

from layer import *

# TODO, Add output layer name to manually stop at some layer
@click.command()
@click.argument('path', type =click.Path(exists=True, file_okay=True, dir_okay=False))
@click.argument('uuid', type = click.INT)
@click.argument('appid', type = click.INT)
@click.argument('export', type=click.Path(exists=True, file_okay=False, dir_okay=True))
@click.option('-t', '--test', type = click.BOOL)
def lf_generate_snapshot_cmd(path, uuid, appid, export, test):
    return lf_generate_snapshot(path, uuid, appid, export, test)

def lf_generate_snapshot(path, uuid, appid, export, test):
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
    extra_include_list = []
    for op_index in range(0, num_ops):
        op = graph.Operators(op_index)
        op_code = model.OperatorCodes(op.OpcodeIndex())
        if op_code.BuiltinCode() == tflite.BuiltinOperator.FULLY_CONNECTED:
            assert(op.InputsLength() == 3)
            input_tensor, input_buffer = get_tensor_and_buffer(model, graph, op.Inputs(0))
            weight_tensor, weight_buffer = get_tensor_and_buffer(model, graph, op.Inputs(1))
            bias_tensor, bias_buffer = get_tensor_and_buffer(model, graph, op.Inputs(2))
            output_tensor, output_buffer = get_tensor_and_buffer(model, graph, op.Outputs(0))

            layer = FCLayer(op_code, input_tensor, weight_tensor, bias_tensor, output_tensor,
                                        input_buffer, weight_buffer, bias_buffer, output_buffer)
            layer_list.append(layer)

        elif op_code.BuiltinCode() == tflite.BuiltinOperator.TANH:
            assert(op.InputsLength() == 1)
            assert(op.OutputsLength() == 1)
            input_tensor, input_buffer = get_tensor_and_buffer(model, graph, op.Inputs(0))
            output_tensor, output_buffer = get_tensor_and_buffer(model, graph, op.Outputs(0))

            layer = TanhLayer(op_code, input_tensor, output_tensor, input_buffer, output_buffer)
            layer_list.append(layer)
            
            if 'tanh_lookup_table.h' not in extra_include_list:
                extra_include_list.append('tanh_lookup_table.h')

        
        elif op_code.BuiltinCode() == tflite.BuiltinOperator.QUANTIZE:
            assert(op.InputsLength() == 1)
            assert(op.OutputsLength() == 1)
            input_tensor, input_buffer = get_tensor_and_buffer(model, graph, op.Inputs(0))
            output_tensor, output_buffer = get_tensor_and_buffer(model, graph, op.Outputs(0))

            layer = QuanLayer(op_code, input_tensor, output_tensor, input_buffer, output_buffer)
            layer_list.append(layer)

        elif op_code.BuiltinCode() == tflite.BuiltinOperator.DEQUANTIZE:
            assert(op.InputsLength() == 1)
            assert(op.OutputsLength() == 1)
            input_tensor, input_buffer = get_tensor_and_buffer(model, graph, op.Inputs(0))
            output_tensor, output_buffer = get_tensor_and_buffer(model, graph, op.Outputs(0))

            layer = DeQuanLayer(op_code, input_tensor, output_tensor, input_buffer, output_buffer)
            layer_list.append(layer)

        else:
            click.echo("Unsupported OP Code: %s ..." % op_code.BuiltinCode())
            continue

    model_input_size = layer_list[0].input_size
    model_output_size = layer_list[-1].output_size

    TEMPLATE_FILE = "main.c"
    _template = template.get_template(TEMPLATE_FILE)
    code = _template.render(model_uuid = uuid,
                            app_id = appid,
                            layer_list = layer_list,
                            input_size = model_input_size,
                            output_size = model_output_size,
                            extra_include_list = extra_include_list,
                            test_mode = test)

    OUTPUT_FILE = f"{export}/lf_model_{uuid}.c"
    with open(OUTPUT_FILE, "w") as output_file:
        output_file.write(code)

def get_tensor_and_buffer(model, graph, input):
    tensor = graph.Tensors(input)
    tensor_type = tensor.Type()
    raw_buffer = model.Buffers(tensor.Buffer()).DataAsNumpy()

    if tensor_type == tflite.TensorType.FLOAT32:
        viewer = '<f4'		
    elif tensor_type == tflite.TensorType.INT8:
        viewer = '<i1'		
    elif tensor_type == tflite.TensorType.INT32:
        viewer = '<i4'		
    else:
        raise Exception('Unsupported Tensor Type: %s ...' % tensor_type)

    if isinstance(raw_buffer, np.ndarray):
        buffer = raw_buffer.view(viewer)
    else:
        buffer = None

    return tensor, buffer

if __name__ == "__main__":
    lf_generate_snapshot_cmd()
