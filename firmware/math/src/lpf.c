#include "efload_math/lpf.h"

#include <math.h>
#include "efload_math/constants.h"

void lpf1_init(lpf1_t *filter, float cutoff_hz, float ts, float initial)
{
    if (filter == 0) {
        return;
    }

    const float tau = 1.0f / (EFLOAD_TWO_PI_F * cutoff_hz);
    filter->alpha = ts / (tau + ts);
    filter->y = initial;
}

float lpf1_step(lpf1_t *filter, float input)
{
    if (filter == 0) {
        return input;
    }

    filter->y += filter->alpha * (input - filter->y);
    return filter->y;
}

