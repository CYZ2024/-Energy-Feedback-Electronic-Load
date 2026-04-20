#include "efload_control/cvcf_source.h"

#include "config_default.h"
#include "efload_math/clarke_park.h"
#include "efload_math/constants.h"

void cvcf_source_init(cvcf_source_t *source, float ts, float grid_hz)
{
    if (source == 0) {
        return;
    }

    source->ts = ts;
    source->omega_rad_s = EFLOAD_TWO_PI_F * grid_hz;
    source->theta_rad = 0.0f;
    source->vd_ref_v = EFLOAD_DEFAULT_LINE_RMS_V * 1.41421356237f;
    source->vq_ref_v = 0.0f;
    source->duty_limits.min = EFLOAD_DEFAULT_DUTY_MIN;
    source->duty_limits.max = EFLOAD_DEFAULT_DUTY_MAX;

    pi_init(&source->voltage_d_pi, 0.10f, 40.0f, ts, -EFLOAD_DEFAULT_LOAD_CURRENT_MAX_A,
            EFLOAD_DEFAULT_LOAD_CURRENT_MAX_A);
    pi_init(&source->voltage_q_pi, 0.10f, 40.0f, ts, -EFLOAD_DEFAULT_LOAD_CURRENT_MAX_A,
            EFLOAD_DEFAULT_LOAD_CURRENT_MAX_A);
    dq_current_loop_init(&source->current_loop, 4.0f, 600.0f, ts, -EFLOAD_DEFAULT_VDC_MAX_V,
                         EFLOAD_DEFAULT_VDC_MAX_V, EFLOAD_DEFAULT_FILTER_L_H);
    source->current_loop.omega_rad_s = source->omega_rad_s;
}

void cvcf_source_reset(cvcf_source_t *source)
{
    if (source == 0) {
        return;
    }

    source->theta_rad = 0.0f;
    pi_reset(&source->voltage_d_pi);
    pi_reset(&source->voltage_q_pi);
    dq_current_loop_reset(&source->current_loop);
}

cvcf_source_output_t cvcf_source_step(cvcf_source_t *source, cvcf_source_input_t input)
{
    cvcf_source_output_t output;
    output.duty.a = 0.5f;
    output.duty.b = 0.5f;
    output.duty.c = 0.5f;
    output.voltage_cmd_v.alpha = 0.0f;
    output.voltage_cmd_v.beta = 0.0f;
    output.theta_rad = 0.0f;
    output.limited = false;
    output.status = CTRL_FAULT_INVALID;

    if (source == 0) {
        return output;
    }

    if (input.dc_bus_v <= 0.0f) {
        output.status = CTRL_FAULT_UV;
        return output;
    }

    source->theta_rad = wrap_angle_rad(source->theta_rad + (source->omega_rad_s * source->ts));
    output.theta_rad = source->theta_rad;

    const alphabeta_t v_ab = clarke_amp_invariant(input.phase_voltage_v);
    const alphabeta_t i_ab = clarke_amp_invariant(input.phase_current_a);
    const dq_t v_dq = park_transform(v_ab, source->theta_rad);
    const dq_t i_dq = park_transform(i_ab, source->theta_rad);

    dq_current_loop_input_t current_input;
    current_input.current_ref_a.d = pi_step(&source->voltage_d_pi, source->vd_ref_v, v_dq.d);
    current_input.current_ref_a.q = pi_step(&source->voltage_q_pi, source->vq_ref_v, v_dq.q);
    current_input.current_meas_a = i_dq;
    current_input.feedforward_v = v_dq;

    const dq_t v_cmd_dq = dq_current_loop_step(&source->current_loop, current_input);
    output.voltage_cmd_v = inverse_park_transform(v_cmd_dq, source->theta_rad);
    const svpwm_result_t pwm = svpwm_compute(output.voltage_cmd_v, input.dc_bus_v, source->duty_limits);
    output.duty = pwm.duty;
    output.limited = pwm.limited;
    output.status = pwm.limited ? CTRL_LIMITED : CTRL_OK;
    return output;
}

