static void tanh_{{ prefix }}_comp (s64 *input, s64 *output)
{
    // Input Scale: {{ input_scale }}
    // Output Scale: {{ output_scale }}
    // Input Offset: {{ input_offset }}
    // Output Offset: {{ output_offset }}
    {% for i in range(0, input_size) %}
    output[{{ i }}] = lf_tanh((input_size[{{ i }}] + {{ input_offset}}) * TANH_X_SCALE_UP * /* input_scale */ ）/ TANH_Y_SCALE_DOWN / /* output_scale */;    
    printk(KERN_INFO "Output_tanh_{{ prefix }}_%d: %lld", {{ i }}, output[{{ i }}]);
    
    {% endfor %}
}