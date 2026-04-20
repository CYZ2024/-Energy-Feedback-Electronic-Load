#ifndef EFLOAD_APP_SUPERVISOR_H
#define EFLOAD_APP_SUPERVISOR_H

#include <stdbool.h>
#include <stdint.h>
#include "efload_control/cc_rectifier.h"
#include "efload_control/cvcf_source.h"
#include "efload_hal/hal_adc.h"
#include "efload_hal/hal_gpio.h"
#include "efload_hal/hal_protection.h"
#include "efload_hal/hal_pwm.h"
#include "efload_hal/hal_timer.h"

typedef enum {
    APP_STATE_INIT = 0,
    APP_STATE_PRECHARGE,
    APP_STATE_SYNC,
    APP_STATE_RUN,
    APP_STATE_FAULT
} app_state_t;

enum {
    APP_FAULT_NONE = 0u,
    APP_FAULT_HAL = 1u << 0,
    APP_FAULT_PROTECTION = 1u << 1,
    APP_FAULT_DC_BUS = 1u << 2,
    APP_FAULT_OVERCURRENT = 1u << 3,
    APP_FAULT_PLL = 1u << 4,
    APP_FAULT_ESTOP = 1u << 5
};

typedef struct {
    float ts;
    float dc_bus_min_v;
    float dc_bus_max_v;
    float phase_current_limit_a;
    uint32_t precharge_us;
    uint32_t sync_locked_ticks;
} supervisor_config_t;

typedef struct {
    hal_adc_t adc;
    hal_pwm_t pwm;
    hal_timer_t timer;
    hal_gpio_t gpio;
    hal_protection_t protection;
} supervisor_io_t;

typedef struct {
    supervisor_config_t config;
    supervisor_io_t io;
    app_state_t state;
    uint32_t fault_flags;
    uint32_t state_entered_us;
    uint32_t sync_ticks;
    cvcf_source_t cvcf;
    cc_rectifier_t rectifier;
} supervisor_t;

void supervisor_default_config(supervisor_config_t *config);
void supervisor_init(supervisor_t *supervisor, const supervisor_config_t *config, supervisor_io_t io);
void supervisor_request_start(supervisor_t *supervisor);
void supervisor_request_stop(supervisor_t *supervisor);
void supervisor_clear_fault(supervisor_t *supervisor);
void supervisor_on_pwm_period(supervisor_t *supervisor);

#endif

