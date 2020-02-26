# load saved model from stable-baseline library
# stable-baseline is for RL
# here we will extract the policy network

import click
import json
import itertools

from layer import *

__author__ = "Junxue ZHANG"

def key_func(str):
    strs = str.split('/')
    ret_str = "{}/{}".format(strs[0], strs[1])
    return ret_str

@click.command()
@click.argument('model_folder_path', type=click.Path(exists=True))
def sb_loader(model_folder_path):
    click.echo("Reading model from %s ..." % click.format_filename(model_folder_path))
    paramter_list_file = model_folder_path + "/parameter_list";
    itr = None
    with open(paramter_list_file) as f:
        itr = json.load(f)

    # remove value function related layer
    itr = filter(lambda paramter: 'vf' not in paramter, itr)
    # remove q
    itr = filter(lambda paramter: 'q' not in paramter, itr)

    # form dict
    paramter_list = itertools.groupby(itr, key=key_func)

    # echo model structure
    # for key, value in paramter_list:
    #     click.echo("layer: %s" % key)
    #     click.echo("paramters: %s" % list(value))

    layer_list = []
    # constuct layers
    for key, value in paramter_list:
        if 'fc' in key:
            layer = FCLayer(model_folder_path, key.split('/')[1], value)
        else:
            # Should be error
            layer = Layer(model_folder_path, key.split('/')[1], value)
        layer_list.append(layer)

    for layer in layer_list:
        layer.generate_comp_c_code()
        break

if __name__ == "__main__":
    sb_loader()