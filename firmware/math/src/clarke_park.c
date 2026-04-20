#include "efload_math/clarke_park.h"

#include <math.h>
#include "efload_math/constants.h"

alphabeta_t clarke_amp_invariant(abc_t abc)
{
    alphabeta_t result;
    result.alpha = abc.a;
    result.beta = (abc.a + (2.0f * abc.b)) * EFLOAD_INV_SQRT3_F;
    return result;
}

abc_t inverse_clarke_amp_invariant(alphabeta_t alphabeta)
{
    abc_t result;
    result.a = alphabeta.alpha;
    result.b = (-0.5f * alphabeta.alpha) + (0.5f * EFLOAD_SQRT3_F * alphabeta.beta);
    result.c = (-0.5f * alphabeta.alpha) - (0.5f * EFLOAD_SQRT3_F * alphabeta.beta);
    return result;
}

dq_t park_transform(alphabeta_t alphabeta, float theta_rad)
{
    const float s = sinf(theta_rad);
    const float c = cosf(theta_rad);
    dq_t result;
    result.d = (alphabeta.alpha * c) + (alphabeta.beta * s);
    result.q = (-alphabeta.alpha * s) + (alphabeta.beta * c);
    return result;
}

alphabeta_t inverse_park_transform(dq_t dq, float theta_rad)
{
    const float s = sinf(theta_rad);
    const float c = cosf(theta_rad);
    alphabeta_t result;
    result.alpha = (dq.d * c) - (dq.q * s);
    result.beta = (dq.d * s) + (dq.q * c);
    return result;
}

float wrap_angle_rad(float angle_rad)
{
    while (angle_rad >= EFLOAD_TWO_PI_F) {
        angle_rad -= EFLOAD_TWO_PI_F;
    }
    while (angle_rad < 0.0f) {
        angle_rad += EFLOAD_TWO_PI_F;
    }
    return angle_rad;
}

float clamp_f32(float value, float min_value, float max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

