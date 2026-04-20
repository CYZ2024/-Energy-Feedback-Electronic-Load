#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "efload_math/pi.h"

static void expect_true(bool condition, const char *name)
{
    if (!condition) {
        fprintf(stderr, "%s\n", name);
        exit(1);
    }
}

int main(void)
{
    pi_controller_t pi;
    pi_init(&pi, 0.8f, 120.0f, 0.001f, -10.0f, 10.0f);

    float y = 0.0f;
    for (uint32_t i = 0u; i < 500u; ++i) {
        const float u = pi_step(&pi, 1.0f, y);
        y += 0.02f * (u - y);
    }
    expect_true(fabsf(1.0f - y) < 0.05f, "pi should remove steady-state error");

    pi_init(&pi, 2.0f, 100.0f, 0.001f, -1.0f, 1.0f);
    for (uint32_t i = 0u; i < 100u; ++i) {
        const float u = pi_step(&pi, 10.0f, 0.0f);
        expect_true(fabsf(u - 1.0f) < 1.0e-6f, "pi high clamp");
    }
    expect_true(pi.integ < 0.5f, "pi conditional integration prevents windup");

    puts("test_pi passed");
    return 0;
}
