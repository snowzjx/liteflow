static void tanh_{{ prefix }}_comp (s64 *input, s64 *output)
{
    // Input Scale: {{ input_scale_numerator }} / {{ input_scale_denominator }}
    // Output Scale: {{ output_scale_numerator }} / {{ output_scale_denominator }}
    // Input Offset: {{ input_offset }}
    // Output Offset: {{ output_offset }}
    {% for i in range(0, input_size) %}
    output[{{ i }}] = lf_tanh((input[{{ i }}] + {{ input_offset}}) * TANH_X_SCALE_UP * {{ input_scale_numerator }} / {{ input_scale_denominator }}) * {{ output_scale_denominator }} / {{ output_scale_numerator }} / TANH_Y_SCALE_DOWN;    
    {% if test_mode %}  
    printk(KERN_INFO "Output_tanh_{{ prefix }}_%d: %lld", {{ i }}, output[{{ i }}]);
    {% endif %}
    {% endfor %}
}