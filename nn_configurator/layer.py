import numpy as np
import jinja2

class Layer:
    weights = None
    bias = None
    name = None
    path = None

    def __init__(self, path, name, ops):
        self.path = path
        self.name = name

        for op in ops:
            if "w:0" in op:
                self.weights = np.load("{}/{}.npy".format(path, op))
            if "b:0" in op:
                self.bias = np.load("{}/{}.npy".format(path, op))

    def generate_comp_c_code(self):
        pass
    
    def print(self):
        print(self.weights)
        print(self.bias)

class FCLayer(Layer):
    def generate_comp_c_code(self):
        weight_input_size = self.weights.shape[0]
        weight_output_size = self.weights.shape[1]
        bias_size = self.bias.shape[0]
        assert(weight_output_size == bias_size)

        print("The input/output size of weight of {} is {} and {}". format(self.name, weight_input_size, weight_output_size))
        print("The size of bias of {} is {}". format(self.name, bias_size))
        
        templateLoader = jinja2.FileSystemLoader(searchpath="nn_configurator/code_template")
        templateEnv = jinja2.Environment(loader=templateLoader)
        TEMPLATE_FILE = "fc_layer_comp.c"
        template = templateEnv.get_template(TEMPLATE_FILE)

        code = template.render(layer_name=self.name,
                                input_size = weight_input_size,
                                output_size = weight_output_size,
                                weights = self.weights,
                                bias = self.bias)
        print(code)
