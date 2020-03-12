static void fc_{{ prefix }}_comp (s64 *input, s64 *output)
{
    {% for i in range(0, output_size) %}
    output[{{ i }}] = 
        {%- for j in range(0, input_size) -%} 
            (input[{{ j }}] + {{ input_offset }}) * ({{ weights[i][j] }} + {{ weight_offset }})
            {%- if not loop.last %} + {% endif -%}
        {%- endfor %} + ({{ bias[i] }});
    output[{{ i }}] = output[{{ i }}] * {{ multiplier }} / 1000000 + {{ output_offset }};

    printk(KERN_INFO "Output_fc_{{ prefix }}_%d: %lld", {{ i }}, output[{{ i }}]);

    {% endfor %}
}