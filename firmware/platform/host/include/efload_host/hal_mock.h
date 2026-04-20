#ifndef EFLOAD_HOST_HAL_MOCK_H
#define EFLOAD_HOST_HAL_MOCK_H

#include "efload_app/supervisor.h"

typedef struct {
    hal_adc_frame_t adc_frame;
    hal_pwm_duties_t last_duties;
    uint32_t now_us;
    uint32_t protection_flags;
    uint32_t disable_count;
    uint32_t update_count;
    bool cvcf_enabled;
    bool rectifier_enabled;
    bool estop;
    bool fail_adc;
    bool fail_pwm_update;
} host_hal_mock_t;

void host_hal_mock_init(host_hal_mock_t *mock);
supervisor_io_t host_hal_mock_make_io(host_hal_mock_t *mock);

#endif

