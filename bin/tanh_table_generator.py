from math import tanh
import template

TAHN_P_RANGE = 300
X_SCALE_DOWN = 100.0
Y_SCALE_UP = 100

tanh_table = [None] * TAHN_P_RANGE;
for x_100 in range (0, TAHN_P_RANGE):
    x = x_100 / X_SCALE_DOWN
    tanh_table[x_100] = tanh(x) * Y_SCALE_UP

TEMPLATE_FILE = "tanh_lookup_table.h"
_template = template.get_template(TEMPLATE_FILE)
code = _template.render(tanh_p_range = TAHN_P_RANGE,
                        tanh_x_scale_up = int(X_SCALE_DOWN),
                        tanh_y_scale_down = Y_SCALE_UP,
                        tanh_table = tanh_table)

OUTPUT_FILE = f"/tmp/tanh_lookup_table.h"
with open(OUTPUT_FILE, "w") as output_file:
    output_file.write(code)