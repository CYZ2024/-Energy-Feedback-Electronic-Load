#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "efload_math/clarke_park.h"
#include "efload_math/constants.h"

static void expect_close(float actual, float expected, float tol, const char *name)
{
    if (fabsf(actual - expected) > tol) {
        fprintf(stderr, "%s expected %.7f got %.7f\n", name, (double)expected, (double)actual);
        exit(1);
    }
}

int main(void)
{
    const abc_t abc = {1.0f, -0.5f, -0.5f};
    const alphabeta_t ab = clarke_amp_invariant(abc);
    expect_close(ab.alpha, 1.0f, 1.0e-6f, "alpha");
    expect_close(ab.beta, 0.0f, 1.0e-6f, "beta");

    const alphabeta_t input = {0.3f, -0.8f};
    const dq_t dq = park_transform(input, 1.1f);
    const alphabeta_t round_trip = inverse_park_transform(dq, 1.1f);
    expect_close(round_trip.alpha, input.alpha, 1.0e-6f, "park alpha round trip");
    expect_close(round_trip.beta, input.beta, 1.0e-6f, "park beta round trip");

    const dq_t d_axis = {1.0f, 0.0f};
    const abc_t phases = inverse_clarke_amp_invariant(inverse_park_transform(d_axis, 0.0f));
    expect_close(phases.a, 1.0f, 1.0e-6f, "d-axis phase a peak");
    expect_close(phases.b, -0.5f, 1.0e-6f, "d-axis phase b peak");
    expect_close(phases.c, -0.5f, 1.0e-6f, "d-axis phase c peak");

    expect_close(wrap_angle_rad(EFLOAD_TWO_PI_F + 0.2f), 0.2f, 1.0e-6f, "wrap high");
    puts("test_clarke_park passed");
    return 0;
}
