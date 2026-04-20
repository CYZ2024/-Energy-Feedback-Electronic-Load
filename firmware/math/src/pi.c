#include "efload_math/pi.h"

#include "efload_math/clarke_park.h"

void pi_init(pi_controller_t *pi, float kp, float ki, float ts, float out_min, float out_max)
{
    if (pi == 0) {
        return;
    }

    pi->kp = kp;
    pi->ki = ki;
    pi->ts = ts;
    pi->out_min = out_min;
    pi->out_max = out_max;
    pi_reset(pi);
}

void pi_reset(pi_controller_t *pi)
{
    if (pi == 0) {
        return;
    }

    pi->integ = 0.0f;
    pi->prev_error = 0.0f;
    pi->last_output = 0.0f;
}

float pi_step(pi_controller_t *pi, float reference, float feedback)
{
    if ((pi == 0) || (pi->ts <= 0.0f) || (pi->out_min > pi->out_max)) {
        return 0.0f;
    }

    const float error = reference - feedback;
    const float candidate_integ = pi->integ + (0.5f * pi->ki * pi->ts * (error + pi->prev_error));
    const float unclamped = (pi->kp * error) + candidate_integ;
    const float output = clamp_f32(unclamped, pi->out_min, pi->out_max);

    const bool saturated_high = (unclamped > pi->out_max) && (error > 0.0f);
    const bool saturated_low = (unclamped < pi->out_min) && (error < 0.0f);

    if (!saturated_high && !saturated_low) {
        pi->integ = candidate_integ;
    }

    pi->prev_error = error;
    pi->last_output = output;
    return output;
}

