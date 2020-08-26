from math import frexp
import template

# Keep four digits after decimal point
KERNEL_PRECISION = 4

class Layer:
    op = None 
    input_size = 0
    output_size = 0

    def __init__(self, op):
        self.op = op

    def generate_struct_code(self, prefix):
        raise NotImplementedError
    
    def generate_comp_code(self, prefix, test_mode):
        raise NotImplementedError
    
    def get_comp_func_name(self, prefix):
        raise NotImplementedError

class FCLayer(Layer):
    weights = None 
    bias = None
    multiplier = 0
    input_offset = 0
    weight_offset = 0
    output_offset = 0

    def __init__(self, op, input_tensor, weight_tensor, bias_tensor, output_tensor,
                            input_buffer, weight_buffer, bias_buffer, output_buffer):
        super().__init__(op)

        assert(input_tensor.Shape(0) == 1)
        assert(output_tensor.Shape(0) == 1)
        assert(input_tensor.Shape(1) == weight_tensor.Shape(1))
        assert(weight_tensor.Shape(0) == bias_tensor.Shape(0))
        assert(output_tensor.Shape(1) == bias_tensor.Shape(0))
        assert(input_buffer == None)
        assert(output_buffer == None)

        self.input_size = input_tensor.Shape(1)
        self.output_size = output_tensor.Shape(1)

        self.weights = weight_buffer
        self.bias = bias_buffer

        self.weights.shape = weight_tensor.ShapeAsNumpy()

        self.input_offset = -input_tensor.Quantization().ZeroPoint(0)
        self.weight_offset = -weight_tensor.Quantization().ZeroPoint(0)
        self.output_offset = output_tensor.Quantization().ZeroPoint(0)

        input_product_scale = input_tensor.Quantization().Scale(0) * weight_tensor.Quantization().Scale(0)
        real_scale = input_product_scale /  output_tensor.Quantization().Scale(0)
        
        self.mantissa_numerator, self.mantissa_denominator, self.exponent = get_quan_multiplier(real_scale)
    
    def generate_struct_code(self, prefix):
        TEMPLATE_FILE = "fc_layer_struct.c"
        _template = template.get_template(TEMPLATE_FILE)
        code = _template.render(prefix = prefix,
                                uuid = prefix, 
                                input_size = self.input_size,
                                output_size = self.output_size)
        return code

    def generate_comp_code(self, prefix, test_mode):
        TEMPLATE_FILE = "fc_layer_comp.c"
        _template = template.get_template(TEMPLATE_FILE)
        code = _template.render(prefix=prefix,
                                input_size = self.input_size,
                                output_size = self.output_size,
                                weights = self.weights,
                                bias = self.bias,
                                input_offset = self.input_offset,
                                weight_offset = self.weight_offset,
                                output_offset = self.output_offset,
                                mantissa_numerator = self.mantissa_numerator,
                                mantissa_denominator = self.mantissa_denominator,
                                exponent = self.exponent,
                                test_mode = test_mode)
        return code
    
    def get_comp_func_name(self, prefix):
        # TODO: make it more flexible
        return "fc_{}_comp".format(prefix)

class TanhLayer(Layer):
    
    input_offset = 0
    output_offset = 0
    input_scale = 0
    output_scale = 0
    
    def __init__(self, op, input_tensor, output_tensor, 
                            input_buffer, output_buffer):
        super().__init__(op)

        assert(input_tensor.Shape(0) == 1)
        assert(output_tensor.Shape(0) == 1)
        assert(input_tensor.Shape(1) == output_tensor.Shape(1))

        assert(input_buffer == None)
        assert(output_buffer == None)

        self.input_size = input_tensor.Shape(1)
        self.output_size = output_tensor.Shape(1)

        self.input_scale = input_tensor.Quantization().Scale(0)
        self.output_scale = output_tensor.Quantization().Scale(0)

        # TODO Check
        self.input_offset = -input_tensor.Quantization().ZeroPoint(0)
        self.output_offset = output_tensor.Quantization().ZeroPoint(0)


    def generate_struct_code(self, prefix):
        TEMPLATE_FILE = "tanh_layer_struct.c"
        _template = template.get_template(TEMPLATE_FILE)
        code = _template.render(prefix = prefix,
                                uuid = prefix, 
                                input_size = self.input_size,
                                output_size = self.output_size)
        return code

    def generate_comp_code(self, prefix, test_mode):
        TEMPLATE_FILE = "tanh_layer_comp.c"
        _template = template.get_template(TEMPLATE_FILE)
        input_scale_numerator, input_scale_denominator = fractionation(self.input_scale)
        output_scale_numerator, output_scale_denominator = fractionation(self.output_scale)
        code = _template.render(prefix=prefix,
                                input_size = self.input_size,
                                output_size = self.output_size,
                                input_offset = self.input_offset,
                                output_offset = self.output_offset,
                                input_scale_numerator = input_scale_numerator,
                                input_scale_denominator = input_scale_denominator,
                                output_scale_numerator = output_scale_numerator,
                                output_scale_denominator = output_scale_denominator,
                                test_mode = test_mode)
        return code
    
    def get_comp_func_name(self, prefix):
        # TODO: make it more flexible
        return "tanh_{}_comp".format(prefix)

class QuanLayer(Layer):

    q_min = 0
    q_max = 0
    zero_point = 0
    
    def __init__(self, op, input_tensor, output_tensor, 
                            input_buffer, output_buffer):
        
        super().__init__(op)

        assert(input_tensor.Shape(0) == 1)
        assert(output_tensor.Shape(0) == 1)
        assert(input_tensor.Shape(1) == output_tensor.Shape(1))

        self.input_size = input_tensor.Shape(1)
        self.output_size = output_tensor.Shape(1)

        self.q_min = output_tensor.Quantization().Min(0)
        self.q_max = output_tensor.Quantization().Max(0)

        self.zero_point = output_tensor.Quantization().ZeroPoint(0)

    def generate_struct_code(self, prefix):
        TEMPLATE_FILE = "quan_layer_struct.c"
        _template = template.get_template(TEMPLATE_FILE)
        code = _template.render(prefix = prefix,
                                uuid = prefix, 
                                input_size = self.input_size,
                                output_size = self.output_size)
        return code

    def generate_comp_code(self, prefix, test_mode):
        TEMPLATE_FILE = "quan_layer_comp.c"
        _template = template.get_template(TEMPLATE_FILE)
        code = _template.render(prefix=prefix,
                                input_size = self.input_size,
                                output_size = self.output_size,
                                q_min = self.q_min,
                                q_max = self.q_max,
                                zero_point = self.zero_point,
                                test_mode = test_mode)
        return code
    
    def get_comp_func_name(self, prefix):
        # TODO: make it more flexible
        return "quan_{}_comp".format(prefix)

class DeQuanLayer(Layer):

    zero_point = 0
    scale = 0
    
    def __init__(self, op, input_tensor, output_tensor, 
                            input_buffer, output_buffer):
        
        super().__init__(op)

        assert(input_tensor.Shape(0) == 1)
        assert(output_tensor.Shape(0) == 1)
        assert(input_tensor.Shape(1) == output_tensor.Shape(1))

        self.input_size = input_tensor.Shape(1)
        self.output_size = output_tensor.Shape(1)

        self.zero_point = input_tensor.Quantization().ZeroPoint(0)
        self.scale = input_tensor.Quantization().Scale(0)

    def generate_struct_code(self, prefix):
        TEMPLATE_FILE = "dequan_layer_struct.c"
        _template = template.get_template(TEMPLATE_FILE)
        code = _template.render(prefix = prefix,
                                uuid = prefix, 
                                input_size = self.input_size,
                                output_size = self.output_size)
        return code

    def generate_comp_code(self, prefix, test_mode):
        TEMPLATE_FILE = "dequan_layer_comp.c"
        _template = template.get_template(TEMPLATE_FILE)
        scale_numerator, scale_denominator, scale_exponent = get_quan_multiplier(self.scale)
        code = _template.render(prefix=prefix,
                                input_size = self.input_size,
                                output_size = self.output_size,
                                zero_point = self.zero_point,
                                scale_numerator = scale_numerator,
                                scale_denominator = scale_denominator,
                                scale_exponent = scale_exponent,
                                scale = self.scale,
                                test_mode = test_mode)
        return code
    
    def get_comp_func_name(self, prefix):
        # TODO: make it more flexible
        return "dequan_{}_comp".format(prefix)

class ConcatenationLayer(Layer):
    # TODO
    pass

class SplitLayer(Layer):
    # TODO
    pass

def fractionation(float_num):
    numerator = float_num
    denominator = 1
    while numerator < 1 and denominator < 2**32:
        numerator *= 10**KERNEL_PRECISION
        denominator *= 10**KERNEL_PRECISION
    return int(numerator), int(denominator)

def get_quan_multiplier(multiplier):
    # Return tuple (numerator, denominator, exponent)
    # 'numerator' and 'denominator' form the mantissa, where 'mantissa = numerator / denominator'
    # 'multiplier = mantissa * 2**exponent'

    m, e = frexp(multiplier)
    while abs(e) > 32:
        if e > 0:
            e -= 1
            m *= 2
        elif m > 2:
            e += 1
            m /= 2
        else:
            return 0, 1, 0
            
    numerator, denominator = fractionation(m)
    return numerator, denominator, int(e)
