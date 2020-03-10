static void dequan_{{ prefix }}_comp (s64 *input, s64 *output)
{
    // Q_min: {{ q_min }}
    // Q_max: {{ q_max }}
    {% for i in range(0, input_size) %}
    output[{{ i }}] = (s8)( (input[{{ i }}] + 128) * {{ q_max | int}} / 255 + {{ q_min | int}}); 
    {% endfor %}
}