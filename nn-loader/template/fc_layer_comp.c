static void fc_{{ prefix }}_comp (s64 *input, s64 *output)
{
    {% for i in range(0, output_size) %}
    output[{{ i }}] = (s8)(
        {%- for j in range(0, input_size) -%} 
            input[{{ j }}] * {{ weights[i][j] }}
            {%- if not loop.last %} + {% endif -%}
        {%- endfor -%}  +  {{ bias[i] }});
    {% endfor %}
}