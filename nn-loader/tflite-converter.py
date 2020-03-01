import click
import tensorflow as tf
import numpy as np

@click.command()
@click.argument('saved_model_dir', type=click.Path(exists=True, file_okay=False, dir_okay=True))
@click.argument('dataset_path', type=click.Path(exists=True, file_okay= True, dir_okay=False))
@click.argument('export_path', type=click.Path(exists=False, file_okay=True, dir_okay=False, writable=True))
def tflite_converter(saved_model_dir, dataset_path, export_path):

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

    click.echo("Done ...")


if __name__ == "__main__":
    tflite_converter()