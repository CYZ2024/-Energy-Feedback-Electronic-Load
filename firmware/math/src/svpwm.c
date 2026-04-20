#include "efload_math/svpwm.h"

#include <math.h>
#include "efload_math/clarke_park.h"
#include "efload_math/constants.h"

static duty_abc_t clamp_duties(duty_abc_t duty, limits_f32_t limits, bool *limited)
{
    const duty_abc_t before = duty;
    duty.a = clamp_f32(duty.a, limits.min, limits.max);
    duty.b = clamp_f32(duty.b, limits.min, limits.max);
    duty.c = clamp_f32(duty.c, limits.min, limits.max);
    if ((duty.a != before.a) || (duty.b != before.b) || (duty.c != before.c)) {
        *limited = true;
    }
    return duty;
}

svpwm_result_t svpwm_compute(alphabeta_t v_ref, float vdc, limits_f32_t duty_limits)
{
    svpwm_result_t result;
    result.duty.a = 0.5f;
    result.duty.b = 0.5f;
    result.duty.c = 0.5f;
    result.limited = false;
    result.modulation_index = 0.0f;

    if ((vdc <= 0.0f) || (duty_limits.min < 0.0f) || (duty_limits.max > 1.0f) ||
        (duty_limits.min >= duty_limits.max)) {
        result.limited = true;
        return result;
    }

    const float magnitude = sqrtf((v_ref.alpha * v_ref.alpha) + (v_ref.beta * v_ref.beta));
    const float linear_limit = vdc * EFLOAD_INV_SQRT3_F;
    if (magnitude > linear_limit) {
        const float scale = linear_limit / magnitude;
        v_ref.alpha *= scale;
        v_ref.beta *= scale;
        result.limited = true;
        result.modulation_index = 1.0f;
    } else {
        result.modulation_index = magnitude / linear_limit;
    }

    const abc_t phase_v = inverse_clarke_amp_invariant(v_ref);
    float vmax = phase_v.a;
    float vmin = phase_v.a;

    if (phase_v.b > vmax) {
        vmax = phase_v.b;
    }
    if (phase_v.c > vmax) {
        vmax = phase_v.c;
    }
    if (phase_v.b < vmin) {
        vmin = phase_v.b;
    }
    if (phase_v.c < vmin) {
        vmin = phase_v.c;
    }

    const float zero_sequence = -0.5f * (vmax + vmin);
    duty_abc_t duty;
    duty.a = 0.5f + ((phase_v.a + zero_sequence) / vdc);
    duty.b = 0.5f + ((phase_v.b + zero_sequence) / vdc);
    duty.c = 0.5f + ((phase_v.c + zero_sequence) / vdc);

    result.duty = clamp_duties(duty, duty_limits, &result.limited);
    return result;
}

