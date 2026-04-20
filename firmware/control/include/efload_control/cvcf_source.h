#ifndef EFLOAD_CONTROL_CVCF_SOURCE_H
#define EFLOAD_CONTROL_CVCF_SOURCE_H

#include "efload_control/dq_current_loop.h"
#include "efload_math/svpwm.h"

typedef struct {
    float ts;
    float omega_rad_s;
    float theta_rad;
    float vd_ref_v;
    float vq_ref_v;
    limits_f32_t duty_limits;
    pi_controller_t voltage_d_pi;
    pi_controller_t voltage_q_pi;
    dq_current_loop_t current_loop;
} cvcf_source_t;

typedef struct {
    abc_t phase_voltage_v;
    abc_t phase_current_a;
    float dc_bus_v;
} cvcf_source_input_t;

typedef struct {
    duty_abc_t duty;
    alphabeta_t voltage_cmd_v;
    float theta_rad;
    bool limited;
    ctrl_status_t status;
} cvcf_source_output_t;

void cvcf_source_init(cvcf_source_t *source, float ts, float grid_hz);
void cvcf_source_reset(cvcf_source_t *source);
cvcf_source_output_t cvcf_source_step(cvcf_source_t *source, cvcf_source_input_t input);

#endif

