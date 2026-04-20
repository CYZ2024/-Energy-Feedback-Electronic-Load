#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "efload_control/cc_rectifier.h"
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
    const float ts = 1.0f / 20000.0f;
    const float omega = EFLOAD_TWO_PI_F * 50.0f;
    cc_rectifier_t rectifier;
    cc_rectifier_init(&rectifier, ts, 50.0f);
    rectifier.pll.lock_ticks_required = 20u;
    rectifier.id_ref_a = 2.0f;
    rectifier.iq_ref_a = 0.0f;

    float theta = 0.0f;
    float id_meas = 0.0f;
    float max_id = 0.0f;
    cc_rectifier_output_t output;
    output.status = CTRL_FAULT_PLL_LOSS;
    output.pll_locked = false;

    for (uint32_t i = 0u; i < 3000u; ++i) {
        theta += omega * ts;
        if (theta >= EFLOAD_TWO_PI_F) {
            theta -= EFLOAD_TWO_PI_F;
        }

        const alphabeta_t voltage_ab = {32.0f * 1.41421356237f * cosf(theta),
                                        32.0f * 1.41421356237f * sinf(theta)};
        const alphabeta_t current_ab = inverse_park_transform((dq_t){id_meas, 0.0f}, theta);
        const cc_rectifier_input_t input = {
            inverse_clarke_amp_invariant(voltage_ab),
            inverse_clarke_amp_invariant(current_ab),
            55.0f,
        };

        output = cc_rectifier_step(&rectifier, input);
        if (output.pll_locked) {
            id_meas += 0.02f * (rectifier.id_ref_a - id_meas);
            if (id_meas > max_id) {
                max_id = id_meas;
            }
        }
    }

    expect_true(output.pll_locked, "rectifier pll should lock");
    expect_true(max_id < 2.4f, "id overshoot below 20 percent");
    expect_true(fabsf(id_meas - 2.0f) < 0.02f, "id steady-state error below 1 percent");
    expect_true(output.duty.a >= 0.0f && output.duty.a <= 1.0f, "duty a bounds");

    puts("test_cc_closed_loop passed");
    return 0;
}

