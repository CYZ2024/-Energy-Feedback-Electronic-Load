#include "efload_math/ramp.h"

void ramp_init(ramp_t *ramp, float initial, float rate_per_s, float ts)
{
    if (ramp == 0) {
        return;
    }

    ramp->value = initial;
    ramp->rate_per_s = rate_per_s;
    ramp->ts = ts;
}

float ramp_step(ramp_t *ramp, float target)
{
    if (ramp == 0) {
        return target;
    }

    const float step = ramp->rate_per_s * ramp->ts;
    if (ramp->value < target) {
        ramp->value += step;
        if (ramp->value > target) {
            ramp->value = target;
        }
    } else if (ramp->value > target) {
        ramp->value -= step;
        if (ramp->value < target) {
            ramp->value = target;
        }
    }

    return ramp->value;
}

