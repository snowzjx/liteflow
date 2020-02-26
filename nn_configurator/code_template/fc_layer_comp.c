static void {{ layer_name }}_comp (s64 *input, s64 *output)
{
    {% for i in range(0, output_size) %}
    output[{{ i }}] = 
        {%- for j in range(0, input_size) -%} 
            input[{{ j }}] * {{ weights[j][i] }}
            {%- if not loop.last -%} + {%- endif-%}
        {%- endfor -%}  + {{ bias[i] }};
    {% endfor %}
}