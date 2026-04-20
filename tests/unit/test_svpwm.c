#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "efload_math/constants.h"
#include "efload_math/svpwm.h"

static void expect_true(bool condition, const char *name)
{
    if (!condition) {
        fprintf(stderr, "%s\n", name);
        exit(1);
    }
}

static void expect_close(float actual, float expected, float tol, const char *name)
{
    if (fabsf(actual - expected) > tol) {
        fprintf(stderr, "%s expected %.7f got %.7f\n", name, (double)expected, (double)actual);
        exit(1);
    }
}

int main(void)
{
    const limits_f32_t limits = {0.0f, 1.0f};
    const svpwm_result_t zero = svpwm_compute((alphabeta_t){0.0f, 0.0f}, 60.0f, limits);
    expect_close(zero.duty.a, 0.5f, 1.0e-6f, "zero a");
    expect_close(zero.duty.b, 0.5f, 1.0e-6f, "zero b");
    expect_close(zero.duty.c, 0.5f, 1.0e-6f, "zero c");

    const svpwm_result_t nominal = svpwm_compute((alphabeta_t){20.0f, 8.0f}, 60.0f, limits);
    expect_true(nominal.duty.a >= 0.0f && nominal.duty.a <= 1.0f, "nominal a bounds");
    expect_true(nominal.duty.b >= 0.0f && nominal.duty.b <= 1.0f, "nominal b bounds");
    expect_true(nominal.duty.c >= 0.0f && nominal.duty.c <= 1.0f, "nominal c bounds");

    const svpwm_result_t m1 = svpwm_compute((alphabeta_t){60.0f * EFLOAD_INV_SQRT3_F, 0.0f}, 60.0f, limits);
    expect_true(m1.duty.a >= 0.0f && m1.duty.a <= 1.0f, "m=1 a bounds");

    const svpwm_result_t over = svpwm_compute((alphabeta_t){100.0f, 0.0f}, 60.0f, limits);
    expect_true(over.limited, "overmod should report limited");
    expect_true(over.duty.a >= 0.0f && over.duty.a <= 1.0f, "over a bounds");

    puts("test_svpwm passed");
    return 0;
}
