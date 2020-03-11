static void quan_{{ prefix }}_comp (s64 *input, s64 *output)
{
    // Q_min: {{ q_min }}
    // Q_max: {{ q_max }}
    {% for i in range(0, input_size) %}
    output[{{ i }}] = (input[{{ i }}] - {{ q_min | int }}) * 255 / ({{ q_max | int}} - {{ q_min | int}}) - 128;
    if (output[{{ i }}] > 127) {
        output[{{ i }}] = 127;
    }
    if (output[{{ i }}] < -128) {
        output[{{ i }}] = -128;
    }
    {% endfor %}
}