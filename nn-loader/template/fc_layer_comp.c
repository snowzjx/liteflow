static void {{ prefix }}_comp (double *input, double *output)
{
    {% for i in range(0, output_size) %}
    output[{{ i }}] = 
        {%- for j in range(0, input_size) -%} 
            input[{{ j }}] * {{ weights[i][j] }}
            {%- if not loop.last -%} + {%- endif -%}
        {%- endfor -%}  + {{ bias[i] }};
    {% endfor %}
}