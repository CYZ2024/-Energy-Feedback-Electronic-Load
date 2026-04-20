#include "efload_app/supervisor.h"

#include <math.h>
#include <string.h>
#include "config_default.h"
#include "efload_app/startup_sequence.h"

static uint32_t now_us(const supervisor_t *supervisor)
{
    if ((supervisor == 0) || (supervisor->io.timer.now_us == 0)) {
        return 0u;
    }
    return supervisor->io.timer.now_us(supervisor->io.timer.context);
}

static void disable_pwm(const supervisor_t *supervisor)
{
    if ((supervisor != 0) && (supervisor->io.pwm.disable != 0)) {
        supervisor->io.pwm.disable(supervisor->io.pwm.context);
    }
}

static void set_enable(const supervisor_t *supervisor, converter_id_t converter, bool enable)
{
    if ((supervisor != 0) && (supervisor->io.pwm.enable != 0)) {
        supervisor->io.pwm.enable(converter, enable, supervisor->io.pwm.context);
    }
}

static void enter_fault(supervisor_t *supervisor, uint32_t fault)
{
    if (supervisor == 0) {
        return;
    }

    supervisor->fault_flags |= fault;
    supervisor->state = APP_STATE_FAULT;
    disable_pwm(supervisor);
}

static bool overcurrent(abc_t current, float limit_a)
{
    return (fabsf(current.a) > limit_a) || (fabsf(current.b) > limit_a) ||
           (fabsf(current.c) > limit_a);
}

static bool validate_frame(supervisor_t *supervisor, const hal_adc_frame_t *frame)
{
    if ((supervisor == 0) || (frame == 0)) {
        return false;
    }

    if ((frame->dc_bus_v < supervisor->config.dc_bus_min_v) ||
        (frame->dc_bus_v > supervisor->config.dc_bus_max_v)) {
        enter_fault(supervisor, APP_FAULT_DC_BUS);
        return false;
    }

    if (overcurrent(frame->cvcf_phase_current_a, supervisor->config.phase_current_limit_a) ||
        overcurrent(frame->rectifier_phase_current_a, supervisor->config.phase_current_limit_a)) {
        enter_fault(supervisor, APP_FAULT_OVERCURRENT);
        return false;
    }

    return true;
}

void supervisor_default_config(supervisor_config_t *config)
{
    if (config == 0) {
        return;
    }

    config->ts = EFLOAD_DEFAULT_TS_S;
    config->dc_bus_min_v = EFLOAD_DEFAULT_VDC_MIN_V;
    config->dc_bus_max_v = EFLOAD_DEFAULT_VDC_MAX_V;
    config->phase_current_limit_a = EFLOAD_DEFAULT_PHASE_CURRENT_LIMIT_A;
    config->precharge_us = EFLOAD_DEFAULT_PRECHARGE_US;
    config->sync_locked_ticks = EFLOAD_DEFAULT_SYNC_MIN_LOCKED_TICKS;
}

void supervisor_init(supervisor_t *supervisor, const supervisor_config_t *config, supervisor_io_t io)
{
    supervisor_config_t default_config;

    if (supervisor == 0) {
        return;
    }

    memset(supervisor, 0, sizeof(*supervisor));
    if (config == 0) {
        supervisor_default_config(&default_config);
        supervisor->config = default_config;
    } else {
        supervisor->config = *config;
    }

    supervisor->io = io;
    supervisor->state = APP_STATE_INIT;
    cvcf_source_init(&supervisor->cvcf, supervisor->config.ts, EFLOAD_DEFAULT_GRID_HZ);
    cc_rectifier_init(&supervisor->rectifier, supervisor->config.ts, EFLOAD_DEFAULT_GRID_HZ);
    disable_pwm(supervisor);
}

void supervisor_request_start(supervisor_t *supervisor)
{
    if ((supervisor == 0) || (supervisor->state != APP_STATE_INIT)) {
        return;
    }

    supervisor->state = APP_STATE_PRECHARGE;
    supervisor->state_entered_us = now_us(supervisor);
}

void supervisor_request_stop(supervisor_t *supervisor)
{
    if (supervisor == 0) {
        return;
    }

    disable_pwm(supervisor);
    set_enable(supervisor, CONVERTER_CVCF_SOURCE, false);
    set_enable(supervisor, CONVERTER_CC_RECTIFIER, false);
    supervisor->state = APP_STATE_INIT;
}

void supervisor_clear_fault(supervisor_t *supervisor)
{
    if ((supervisor == 0) || (supervisor->state != APP_STATE_FAULT)) {
        return;
    }

    supervisor->fault_flags = APP_FAULT_NONE;
    cvcf_source_reset(&supervisor->cvcf);
    cc_rectifier_reset(&supervisor->rectifier);
    supervisor->state = APP_STATE_INIT;
}

void supervisor_on_pwm_period(supervisor_t *supervisor)
{
    hal_adc_frame_t frame;

    if (supervisor == 0) {
        return;
    }

    if ((supervisor->io.gpio.read_estop != 0) &&
        supervisor->io.gpio.read_estop(supervisor->io.gpio.context)) {
        enter_fault(supervisor, APP_FAULT_ESTOP);
        return;
    }

    if (supervisor->io.protection.read_latched != 0) {
        const uint32_t protection = supervisor->io.protection.read_latched(supervisor->io.protection.context);
        if (protection != HAL_PROTECTION_NONE) {
            enter_fault(supervisor, APP_FAULT_PROTECTION);
            return;
        }
    }

    if ((supervisor->io.adc.read == 0) ||
        !supervisor->io.adc.read(&frame, supervisor->io.adc.context)) {
        enter_fault(supervisor, APP_FAULT_HAL);
        return;
    }

    switch (supervisor->state) {
    case APP_STATE_INIT:
        disable_pwm(supervisor);
        break;
    case APP_STATE_PRECHARGE:
        if (!validate_frame(supervisor, &frame)) {
            return;
        }
        if (startup_delay_elapsed(now_us(supervisor), supervisor->state_entered_us,
                                  supervisor->config.precharge_us)) {
            supervisor->state = APP_STATE_SYNC;
            supervisor->sync_ticks = 0u;
            cvcf_source_reset(&supervisor->cvcf);
            cc_rectifier_reset(&supervisor->rectifier);
        }
        break;
    case APP_STATE_SYNC: {
        if (!validate_frame(supervisor, &frame)) {
            return;
        }

        const cvcf_source_input_t cvcf_input = {
            frame.cvcf_phase_voltage_v,
            frame.cvcf_phase_current_a,
            frame.dc_bus_v,
        };
        const cvcf_source_output_t cvcf_output = cvcf_source_step(&supervisor->cvcf, cvcf_input);
        const hal_pwm_duties_t duties = {cvcf_output.duty, {0.5f, 0.5f, 0.5f}};

        if ((supervisor->io.pwm.update_duties == 0) ||
            !supervisor->io.pwm.update_duties(&duties, supervisor->io.pwm.context)) {
            enter_fault(supervisor, APP_FAULT_HAL);
            return;
        }
        set_enable(supervisor, CONVERTER_CVCF_SOURCE, cvcf_output.status != CTRL_FAULT_UV);
        set_enable(supervisor, CONVERTER_CC_RECTIFIER, false);

        supervisor->sync_ticks++;
        if (supervisor->sync_ticks >= supervisor->config.sync_locked_ticks) {
            supervisor->state = APP_STATE_RUN;
        }
        break;
    }
    case APP_STATE_RUN: {
        if (!validate_frame(supervisor, &frame)) {
            return;
        }

        const cvcf_source_input_t cvcf_input = {
            frame.cvcf_phase_voltage_v,
            frame.cvcf_phase_current_a,
            frame.dc_bus_v,
        };
        const cc_rectifier_input_t rectifier_input = {
            frame.rectifier_phase_voltage_v,
            frame.rectifier_phase_current_a,
            frame.dc_bus_v,
        };

        const cvcf_source_output_t cvcf_output = cvcf_source_step(&supervisor->cvcf, cvcf_input);
        const cc_rectifier_output_t rectifier_output =
            cc_rectifier_step(&supervisor->rectifier, rectifier_input);

        if (rectifier_output.status == CTRL_FAULT_PLL_LOSS) {
            set_enable(supervisor, CONVERTER_CC_RECTIFIER, false);
        }

        const hal_pwm_duties_t duties = {cvcf_output.duty, rectifier_output.duty};
        if ((supervisor->io.pwm.update_duties == 0) ||
            !supervisor->io.pwm.update_duties(&duties, supervisor->io.pwm.context)) {
            enter_fault(supervisor, APP_FAULT_HAL);
            return;
        }

        set_enable(supervisor, CONVERTER_CVCF_SOURCE, cvcf_output.status != CTRL_FAULT_UV);
        set_enable(supervisor, CONVERTER_CC_RECTIFIER, rectifier_output.pll_locked);
        break;
    }
    case APP_STATE_FAULT:
    default:
        disable_pwm(supervisor);
        break;
    }
}

