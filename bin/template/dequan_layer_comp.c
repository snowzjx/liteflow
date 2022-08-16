static void dequan_{{ prefix }}_comp (s64 *input, s64 *output)
{
    // zero_point: {{ zero_point }}
    // scale: {{ scale }}
    {% for i in range(0, input_size) %}
    output[{{ i }}] = (((input[{{ i }}] - {{ zero_point }}) * {{ scale_numerator | int }} / {{ scale_denominator | int }}) {% if scale_exponent > 0 %} << {{ scale_exponent | int }}{% else %} >> -({{ scale_exponent | int }}){% endif %});
    {% endfor %}
}
