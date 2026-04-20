#ifndef EFLOAD_MATH_RAMP_H
#define EFLOAD_MATH_RAMP_H

typedef struct {
    float value;
    float rate_per_s;
    float ts;
} ramp_t;

void ramp_init(ramp_t *ramp, float initial, float rate_per_s, float ts);
float ramp_step(ramp_t *ramp, float target);

#endif

