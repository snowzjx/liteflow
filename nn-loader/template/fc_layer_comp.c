static void fc_{{ prefix }}_comp (s64 *input, s64 *output)
{
    {% for i in range(0, output_size) %}
    output[{{ i }}] = 
        {%- for j in range(0, input_size) -%} 
            (input[{{ j }}] + {{ input_offset }}) * ({{ weights[i][j] }} + {{ weight_offset }})
            {%- if not loop.last %} + {% endif -%}
        {%- endfor %} + ({{ bias[i] }});
    output[{{ i }}] = ((output[{{ i }}] * {{ mantissa_numerator | int }} / {{ mantissa_denominator | int }}) {% if exponent > 0 %} << {{ exponent | int }}{% else %} >> -({{ exponent | int }}){% endif %}) + {{ output_offset }};
    {% if test_mode %}
    printk(KERN_INFO "Output_fc_{{ prefix }}_%d: %lld", {{ i }}, output[{{ i }}]);
    {% endif %}
    {% endfor %}
}