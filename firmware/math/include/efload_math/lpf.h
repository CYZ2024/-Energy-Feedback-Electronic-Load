#ifndef EFLOAD_MATH_LPF_H
#define EFLOAD_MATH_LPF_H

typedef struct {
    float alpha;
    float y;
} lpf1_t;

void lpf1_init(lpf1_t *filter, float cutoff_hz, float ts, float initial);
float lpf1_step(lpf1_t *filter, float input);

#endif

