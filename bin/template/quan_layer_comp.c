static void quan_{{ prefix }}_comp (s64 *input, s64 *output)
{
    // Q_min: {{ q_min }}
    // Q_max: {{ q_max }}
    {% for i in range(0, input_size) %}
    output[{{ i }}] = (input[{{ i }}] - {{ q_min | int }}) * 255 / {{ (q_max - q_min) | int}} + {{ zero_point }};
    {% if test_mode %}  
    printk(KERN_INFO "Output_quan_{{ prefix }}_%d: %lld", {{ i }}, output[{{ i }}]);
    {% endif %}
    {% endfor %}
}