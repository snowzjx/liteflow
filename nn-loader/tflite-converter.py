import click
import tensorflow as tf
import numpy as np

@click.command()
@click.argument('saved_model_dir', type=click.Path(exists=True, file_okay=False, dir_okay=True))
@click.argument('export_path', type=click.Path(exists=False, file_okay=True, dir_okay=False, writable=True))
def tflite_converter(saved_model_dir, export_path):

    converter = tf.lite.TFLiteConverter.from_saved_model(saved_model_dir)
    converter.allow_custom_ops=True
    converter.target_ops = [tf.lite.OpsSet.TFLITE_BUILTINS,
                            tf.lite.OpsSet.SELECT_TF_OPS]

    converter.optimizations = [tf.lite.Optimize.OPTIMIZE_FOR_SIZE]

    # converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
    # converter.inference_input_type = tf.uint8
    # converter.inference_output_type = tf.uint8

    tflite_model = converter.convert()
    open(export_path, "wb").write(tflite_model)

if __name__ == "__main__":
    tflite_converter()