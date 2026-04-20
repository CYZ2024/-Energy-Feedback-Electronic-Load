#include "efload_host/hal_mock.h"

#include <string.h>

static bool mock_adc_read(hal_adc_frame_t *frame, void *context)
{
    host_hal_mock_t *mock = (host_hal_mock_t *)context;
    if ((mock == 0) || (frame == 0) || mock->fail_adc) {
        return false;
    }

    *frame = mock->adc_frame;
    return true;
}

static bool mock_pwm_update(const hal_pwm_duties_t *duties, void *context)
{
    host_hal_mock_t *mock = (host_hal_mock_t *)context;
    if ((mock == 0) || (duties == 0) || mock->fail_pwm_update) {
        return false;
    }

    mock->last_duties = *duties;
    mock->update_count++;
    return true;
}

static void mock_pwm_enable(converter_id_t converter, bool enable, void *context)
{
    host_hal_mock_t *mock = (host_hal_mock_t *)context;
    if (mock == 0) {
        return;
    }

    if (converter == CONVERTER_CVCF_SOURCE) {
        mock->cvcf_enabled = enable;
    } else {
        mock->rectifier_enabled = enable;
    }
}

static void mock_pwm_disable(void *context)
{
    host_hal_mock_t *mock = (host_hal_mock_t *)context;
    if (mock == 0) {
        return;
    }

    mock->cvcf_enabled = false;
    mock->rectifier_enabled = false;
    mock->disable_count++;
}

static bool mock_pwm_set_deadtime(uint32_t deadtime_ns, void *context)
{
    (void)deadtime_ns;
    (void)context;
    return true;
}

static uint32_t mock_now_us(void *context)
{
    const host_hal_mock_t *mock = (const host_hal_mock_t *)context;
    if (mock == 0) {
        return 0u;
    }
    return mock->now_us;
}

static void mock_led(bool on, void *context)
{
    (void)on;
    (void)context;
}

static bool mock_estop(void *context)
{
    const host_hal_mock_t *mock = (const host_hal_mock_t *)context;
    return (mock != 0) && mock->estop;
}

static uint32_t mock_read_protection(void *context)
{
    const host_hal_mock_t *mock = (const host_hal_mock_t *)context;
    if (mock == 0) {
        return HAL_PROTECTION_NONE;
    }
    return mock->protection_flags;
}

static void mock_clear_protection(void *context)
{
    host_hal_mock_t *mock = (host_hal_mock_t *)context;
    if (mock != 0) {
        mock->protection_flags = HAL_PROTECTION_NONE;
    }
}

void host_hal_mock_init(host_hal_mock_t *mock)
{
    if (mock == 0) {
        return;
    }

    memset(mock, 0, sizeof(*mock));
    mock->adc_frame.dc_bus_v = 55.0f;
}

supervisor_io_t host_hal_mock_make_io(host_hal_mock_t *mock)
{
    supervisor_io_t io;
    io.adc.read = mock_adc_read;
    io.adc.context = mock;
    io.pwm.update_duties = mock_pwm_update;
    io.pwm.enable = mock_pwm_enable;
    io.pwm.disable = mock_pwm_disable;
    io.pwm.set_deadtime_ns = mock_pwm_set_deadtime;
    io.pwm.context = mock;
    io.timer.now_us = mock_now_us;
    io.timer.context = mock;
    io.gpio.set_status_led = mock_led;
    io.gpio.read_estop = mock_estop;
    io.gpio.context = mock;
    io.protection.read_latched = mock_read_protection;
    io.protection.clear_latched = mock_clear_protection;
    io.protection.context = mock;
    return io;
}

