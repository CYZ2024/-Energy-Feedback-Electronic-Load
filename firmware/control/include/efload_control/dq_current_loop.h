#ifndef EFLOAD_CONTROL_DQ_CURRENT_LOOP_H
#define EFLOAD_CONTROL_DQ_CURRENT_LOOP_H

#include "efload_math/pi.h"

typedef struct {
    float filter_l_h;
    float omega_rad_s;
    pi_controller_t d_pi;
    pi_controller_t q_pi;
} dq_current_loop_t;

typedef struct {
    dq_t current_ref_a;
    dq_t current_meas_a;
    dq_t feedforward_v;
} dq_current_loop_input_t;

void dq_current_loop_init(
    dq_current_loop_t *loop,
    float kp,
    float ki,
    float ts,
    float out_min_v,
    float out_max_v,
    float filter_l_h);
void dq_current_loop_reset(dq_current_loop_t *loop);
dq_t dq_current_loop_step(dq_current_loop_t *loop, dq_current_loop_input_t input);

#endif

