#ifndef EFLOAD_HAL_ADC_H
#define EFLOAD_HAL_ADC_H

#include <stdbool.h>
#include "efload_core/types.h"

typedef struct {
    abc_t cvcf_phase_voltage_v;
    abc_t cvcf_phase_current_a;
    abc_t rectifier_phase_voltage_v;
    abc_t rectifier_phase_current_a;
    float dc_bus_v;
} hal_adc_frame_t;

typedef struct {
    bool (*read)(hal_adc_frame_t *frame, void *context);
    void *context;
} hal_adc_t;

#endif

