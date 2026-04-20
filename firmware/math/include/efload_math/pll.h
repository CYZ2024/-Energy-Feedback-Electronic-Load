#ifndef EFLOAD_MATH_PLL_H
#define EFLOAD_MATH_PLL_H

#include <stdbool.h>
#include "efload_core/types.h"
#include "efload_math/pi.h"

typedef struct {
    pi_controller_t pi;
    float theta_rad;
    float omega_rad_s;
    float nominal_omega_rad_s;
    float lock_error_rad;
    uint32_t lock_ticks_required;
    uint32_t lock_ticks;
    bool locked;
} srf_pll_t;

void pll_init(
    srf_pll_t *pll,
    float kp,
    float ki,
    float ts,
    float nominal_omega_rad_s,
    float min_omega_rad_s,
    float max_omega_rad_s);
void pll_reset(srf_pll_t *pll, float theta_rad);
float pll_step(srf_pll_t *pll, alphabeta_t voltage);

#endif

