#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "efload_control/cvcf_source.h"
#include "efload_math/clarke_park.h"
#include "efload_math/constants.h"

static void expect_true(bool condition, const char *name)
{
    if (!condition) {
        fprintf(stderr, "%s\n", name);
        exit(1);
    }
}

int main(void)
{
    cvcf_source_t source;
    cvcf_source_init(&source, 1.0f / 20000.0f, 50.0f);

    float vd_meas = 0.0f;
    cvcf_source_output_t output;
    output.status = CTRL_FAULT_INVALID;

    for (uint32_t i = 0u; i < 6000u; ++i) {
        const alphabeta_t voltage_ab = inverse_park_transform((dq_t){vd_meas, 0.0f}, source.theta_rad);
        const cvcf_source_input_t input = {
            inverse_clarke_amp_invariant(voltage_ab),
            {0.0f, 0.0f, 0.0f},
            55.0f,
        };
        output = cvcf_source_step(&source, input);
        vd_meas += 0.01f * (source.vd_ref_v - vd_meas);
    }

    const float phase_rms = fabsf(vd_meas) * 0.70710678118f;
    expect_true(output.status == CTRL_OK || output.status == CTRL_LIMITED, "cvcf status valid");
    expect_true(fabsf(phase_rms - 32.0f) < (32.0f * 0.02f), "cvcf rms within 2 percent");
    expect_true(output.duty.a >= 0.0f && output.duty.a <= 1.0f, "duty a bounds");

    puts("test_cvcf_closed_loop passed");
    return 0;
}

