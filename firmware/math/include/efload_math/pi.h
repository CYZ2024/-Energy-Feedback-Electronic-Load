#ifndef EFLOAD_MATH_PI_H
#define EFLOAD_MATH_PI_H

#include "efload_core/types.h"

typedef struct {
    float kp;
    float ki;
    float ts;
    float out_min;
    float out_max;
    float integ;
    float prev_error;
    float last_output;
} pi_controller_t;

void pi_init(pi_controller_t *pi, float kp, float ki, float ts, float out_min, float out_max);
void pi_reset(pi_controller_t *pi);
float pi_step(pi_controller_t *pi, float reference, float feedback);

#endif

