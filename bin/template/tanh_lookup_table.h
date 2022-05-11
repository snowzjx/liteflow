#ifndef TANH_LOOPUP_TABLE
#define TANH_LOOPUP_TABLE

#define TAHN_P_RANGE {{ tanh_p_range }}
#define TANH_X_SCALE_UP {{ tanh_x_scale_up }}
#define TANH_Y_SCALE_DOWN {{ tanh_y_scale_down }}

const static s32 tanh_look_up_table[TAHN_P_RANGE] = {
    {% for value in tanh_table -%}
    [{{ loop.index - 1 }}] = {{ value | int}},
    {% endfor -%}
};

static inline s32 lf_tanh (s32 x_100)
{
    if (x_100 == 0) {
        return 0;
    }
    else if (x_100 < 0) 
    {
        return -lf_tanh(-x_100);
    }
    else if (x_100 >= TAHN_P_RANGE) {
        return 1 * TANH_Y_SCALE_DOWN;
    }
    else {
        return tanh_look_up_table[x_100];
    } 
}

#endif