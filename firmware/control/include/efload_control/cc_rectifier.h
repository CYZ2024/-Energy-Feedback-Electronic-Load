#ifndef EFLOAD_CONTROL_CC_RECTIFIER_H
#define EFLOAD_CONTROL_CC_RECTIFIER_H

#include "efload_control/dq_current_loop.h"
#include "efload_math/pll.h"
#include "efload_math/svpwm.h"

typedef enum {
    RECTIFIER_ID_FROM_COMMAND = 0,
    RECTIFIER_ID_FROM_VDC_LOOP = 1
} rectifier_id_mode_t;

typedef struct {
    float ts;
    float id_ref_a;
    float iq_ref_a;
    float vdc_ref_v;
    rectifier_id_mode_t id_mode;
    limits_f32_t duty_limits;
    srf_pll_t pll;
    pi_controller_t vdc_pi;
    dq_current_loop_t current_loop;
} cc_rectifier_t;

typedef struct {
    abc_t phase_voltage_v;
    abc_t phase_current_a;
    float dc_bus_v;
} cc_rectifier_input_t;

typedef struct {
    duty_abc_t duty;
    alphabeta_t voltage_cmd_v;
    float theta_rad;
    bool pll_locked;
    bool limited;
    ctrl_status_t status;
} cc_rectifier_output_t;

void cc_rectifier_init(cc_rectifier_t *rectifier, float ts, float grid_hz);
void cc_rectifier_reset(cc_rectifier_t *rectifier);
cc_rectifier_output_t cc_rectifier_step(cc_rectifier_t *rectifier, cc_rectifier_input_t input);

#endif

