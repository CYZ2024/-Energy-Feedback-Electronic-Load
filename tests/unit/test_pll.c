#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "efload_math/constants.h"
#include "efload_math/pll.h"

static float angle_error(float a, float b)
{
    float e = a - b;
    while (e > EFLOAD_PI_F) {
        e -= EFLOAD_TWO_PI_F;
    }
    while (e < -EFLOAD_PI_F) {
        e += EFLOAD_TWO_PI_F;
    }
    return e;
}

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
    srf_pll_t pll;
    pll_init(&pll, 160.0f, 4000.0f, ts, omega, omega * 0.8f, omega * 1.2f);
    pll.lock_ticks_required = 20u;

    float true_theta = 0.0f;
    for (uint32_t i = 0u; i < 4000u; ++i) {
        true_theta += omega * ts;
        if (true_theta >= EFLOAD_TWO_PI_F) {
            true_theta -= EFLOAD_TWO_PI_F;
        }
        const alphabeta_t v = {cosf(true_theta), sinf(true_theta)};
        (void)pll_step(&pll, v);
    }

    expect_true(pll.locked, "pll should lock");
    expect_true(fabsf(pll.omega_rad_s - omega) < 2.0f, "pll omega should converge");
    expect_true(fabsf(angle_error(pll.theta_rad, true_theta)) < 0.03f, "pll phase should converge");

    true_theta += 0.5f;
    if (true_theta >= EFLOAD_TWO_PI_F) {
        true_theta -= EFLOAD_TWO_PI_F;
    }
    for (uint32_t i = 0u; i < 1000u; ++i) {
        true_theta += omega * ts;
        if (true_theta >= EFLOAD_TWO_PI_F) {
            true_theta -= EFLOAD_TWO_PI_F;
        }
        const alphabeta_t v = {cosf(true_theta), sinf(true_theta)};
        (void)pll_step(&pll, v);
    }

    expect_true(fabsf(angle_error(pll.theta_rad, true_theta)) < 0.06f, "pll phase jump recovery");
    puts("test_pll passed");
    return 0;
}
