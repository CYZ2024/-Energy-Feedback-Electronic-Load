#ifndef EFLOAD_HAL_PWM_H
#define EFLOAD_HAL_PWM_H

#include <stdbool.h>
#include <stdint.h>
#include "efload_core/types.h"

typedef struct {
    duty_abc_t cvcf;
    duty_abc_t rectifier;
} hal_pwm_duties_t;

typedef struct {
    bool (*update_duties)(const hal_pwm_duties_t *duties, void *context);
    void (*enable)(converter_id_t converter, bool enable, void *context);
    void (*disable)(void *context);
    bool (*set_deadtime_ns)(uint32_t deadtime_ns, void *context);
    void *context;
} hal_pwm_t;

#endif

