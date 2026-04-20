#include "efload_control/dq_current_loop.h"

void dq_current_loop_init(
    dq_current_loop_t *loop,
    float kp,
    float ki,
    float ts,
    float out_min_v,
    float out_max_v,
    float filter_l_h)
{
    if (loop == 0) {
        return;
    }

    loop->filter_l_h = filter_l_h;
    loop->omega_rad_s = 0.0f;
    pi_init(&loop->d_pi, kp, ki, ts, out_min_v, out_max_v);
    pi_init(&loop->q_pi, kp, ki, ts, out_min_v, out_max_v);
}

void dq_current_loop_reset(dq_current_loop_t *loop)
{
    if (loop == 0) {
        return;
    }

    pi_reset(&loop->d_pi);
    pi_reset(&loop->q_pi);
}

dq_t dq_current_loop_step(dq_current_loop_t *loop, dq_current_loop_input_t input)
{
    dq_t result;
    result.d = 0.0f;
    result.q = 0.0f;

    if (loop == 0) {
        return result;
    }

    const float vd_star = pi_step(&loop->d_pi, input.current_ref_a.d, input.current_meas_a.d);
    const float vq_star = pi_step(&loop->q_pi, input.current_ref_a.q, input.current_meas_a.q);
    const float omega_l = loop->omega_rad_s * loop->filter_l_h;

    result.d = vd_star - (omega_l * input.current_meas_a.q) + input.feedforward_v.d;
    result.q = vq_star + (omega_l * input.current_meas_a.d) + input.feedforward_v.q;
    return result;
}

