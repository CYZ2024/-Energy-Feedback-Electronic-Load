#ifndef EFLOAD_MATH_SVPWM_H
#define EFLOAD_MATH_SVPWM_H

#include <stdbool.h>
#include "efload_core/types.h"

typedef struct {
    duty_abc_t duty;
    bool limited;
    float modulation_index;
} svpwm_result_t;

svpwm_result_t svpwm_compute(alphabeta_t v_ref, float vdc, limits_f32_t duty_limits);

#endif

