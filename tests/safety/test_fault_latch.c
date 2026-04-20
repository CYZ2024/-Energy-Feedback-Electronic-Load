#include <stdio.h>
#include <stdlib.h>
#include "efload_host/hal_mock.h"

static void expect_true(bool condition, const char *name)
{
    if (!condition) {
        fprintf(stderr, "%s\n", name);
        exit(1);
    }
}

int main(void)
{
    host_hal_mock_t mock;
    host_hal_mock_init(&mock);
    mock.adc_frame.dc_bus_v = 55.0f;

    supervisor_config_t config;
    supervisor_default_config(&config);
    config.precharge_us = 1u;
    config.sync_locked_ticks = 2u;

    supervisor_t supervisor;
    supervisor_init(&supervisor, &config, host_hal_mock_make_io(&mock));
    supervisor_request_start(&supervisor);

    mock.now_us = 2u;
    supervisor_on_pwm_period(&supervisor);
    expect_true(supervisor.state == APP_STATE_SYNC, "precharge to sync");

    mock.protection_flags = HAL_PROTECTION_OVERCURRENT;
    supervisor_on_pwm_period(&supervisor);
    expect_true(supervisor.state == APP_STATE_FAULT, "overcurrent enters fault");
    expect_true((supervisor.fault_flags & APP_FAULT_PROTECTION) != 0u, "protection fault flag");
    expect_true(mock.disable_count > 0u, "fault calls pwm disable");

    supervisor_on_pwm_period(&supervisor);
    expect_true(!mock.cvcf_enabled && !mock.rectifier_enabled, "fault remains disabled");

    mock.protection_flags = HAL_PROTECTION_NONE;
    supervisor_clear_fault(&supervisor);
    expect_true(supervisor.state == APP_STATE_INIT, "explicit clear required");

    puts("test_fault_latch passed");
    return 0;
}

