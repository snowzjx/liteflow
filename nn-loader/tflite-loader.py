import click
import os
import json

_SCHEMA = "third_party/tensorflow/tensorflow/lite/schema/schema.fbs"

_BINARY = "third_party/flatbuffers/flatc"

@click.command()
@click.argument('path', type=click.Path(exists=True, file_okay=True, dir_okay=False))
def tflite_loader(path):
    click.echo("Reading model from %s ..." % click.format_filename(path))
    if not path.endswith('.tflite'):
        click.echo("The file should end with .tflite")
        return 
    
    CMD = (
        _BINARY + " -t "
        "--strict-json --defaults-json -o /tmp {schema} -- {input}".format(
            input=path, schema=_SCHEMA))
    click.echo("running command: %s" % CMD) 
    os.system(CMD)

    json_path = ("/tmp/" + os.path.splitext(
        os.path.split(path)[-1])[0] + ".json")

    model =json.load(open(json_path))

    click.echo('model loaded...')

if __name__ == "__main__":
    tflite_loader()