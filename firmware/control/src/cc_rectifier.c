#include "efload_control/cc_rectifier.h"

#include "config_default.h"
#include "efload_math/clarke_park.h"
#include "efload_math/constants.h"

void cc_rectifier_init(cc_rectifier_t *rectifier, float ts, float grid_hz)
{
    if (rectifier == 0) {
        return;
    }

    rectifier->ts = ts;
    rectifier->id_ref_a = 0.0f;
    rectifier->iq_ref_a = 0.0f;
    rectifier->vdc_ref_v = EFLOAD_DEFAULT_VDC_NOMINAL_V;
    rectifier->id_mode = RECTIFIER_ID_FROM_COMMAND;
    rectifier->duty_limits.min = EFLOAD_DEFAULT_DUTY_MIN;
    rectifier->duty_limits.max = EFLOAD_DEFAULT_DUTY_MAX;

    const float omega = EFLOAD_TWO_PI_F * grid_hz;
    pll_init(&rectifier->pll, 40.0f, 800.0f, ts, omega, omega * 0.8f, omega * 1.2f);
    pi_init(&rectifier->vdc_pi, 0.05f, 10.0f, ts, 0.0f, EFLOAD_DEFAULT_LOAD_CURRENT_MAX_A);
    dq_current_loop_init(&rectifier->current_loop, 4.0f, 600.0f, ts, -EFLOAD_DEFAULT_VDC_MAX_V,
                         EFLOAD_DEFAULT_VDC_MAX_V, EFLOAD_DEFAULT_FILTER_L_H);
    rectifier->current_loop.omega_rad_s = omega;
}

void cc_rectifier_reset(cc_rectifier_t *rectifier)
{
    if (rectifier == 0) {
        return;
    }

    pll_reset(&rectifier->pll, 0.0f);
    pi_reset(&rectifier->vdc_pi);
    dq_current_loop_reset(&rectifier->current_loop);
}

cc_rectifier_output_t cc_rectifier_step(cc_rectifier_t *rectifier, cc_rectifier_input_t input)
{
    cc_rectifier_output_t output;
    output.duty.a = 0.5f;
    output.duty.b = 0.5f;
    output.duty.c = 0.5f;
    output.voltage_cmd_v.alpha = 0.0f;
    output.voltage_cmd_v.beta = 0.0f;
    output.theta_rad = 0.0f;
    output.pll_locked = false;
    output.limited = false;
    output.status = CTRL_FAULT_INVALID;

    if (rectifier == 0) {
        return output;
    }

    if (input.dc_bus_v <= 0.0f) {
        output.status = CTRL_FAULT_UV;
        return output;
    }

    const alphabeta_t v_ab = clarke_amp_invariant(input.phase_voltage_v);
    const float theta = pll_step(&rectifier->pll, v_ab);
    output.theta_rad = theta;
    output.pll_locked = rectifier->pll.locked;

    if (!rectifier->pll.locked) {
        output.status = CTRL_FAULT_PLL_LOSS;
        return output;
    }

    const alphabeta_t i_ab = clarke_amp_invariant(input.phase_current_a);
    const dq_t i_dq = park_transform(i_ab, theta);
    const dq_t v_dq = park_transform(v_ab, theta);

    dq_current_loop_input_t current_input;
    if (rectifier->id_mode == RECTIFIER_ID_FROM_VDC_LOOP) {
        current_input.current_ref_a.d = pi_step(&rectifier->vdc_pi, rectifier->vdc_ref_v, input.dc_bus_v);
    } else {
        current_input.current_ref_a.d = rectifier->id_ref_a;
    }
    current_input.current_ref_a.q = rectifier->iq_ref_a;
    current_input.current_meas_a = i_dq;
    current_input.feedforward_v = v_dq;

    const dq_t v_cmd_dq = dq_current_loop_step(&rectifier->current_loop, current_input);
    output.voltage_cmd_v = inverse_park_transform(v_cmd_dq, theta);
    const svpwm_result_t pwm = svpwm_compute(output.voltage_cmd_v, input.dc_bus_v, rectifier->duty_limits);
    output.duty = pwm.duty;
    output.limited = pwm.limited;
    output.status = pwm.limited ? CTRL_LIMITED : CTRL_OK;
    return output;
}

