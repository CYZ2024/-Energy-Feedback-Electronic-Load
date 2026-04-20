#include "efload_math/pll.h"

#include <math.h>
#include "efload_math/clarke_park.h"

void pll_init(
    srf_pll_t *pll,
    float kp,
    float ki,
    float ts,
    float nominal_omega_rad_s,
    float min_omega_rad_s,
    float max_omega_rad_s)
{
    if (pll == 0) {
        return;
    }

    pi_init(&pll->pi, kp, ki, ts, min_omega_rad_s - nominal_omega_rad_s,
            max_omega_rad_s - nominal_omega_rad_s);
    pll->nominal_omega_rad_s = nominal_omega_rad_s;
    pll->lock_error_rad = 0.02f;
    pll->lock_ticks_required = 100u;
    pll_reset(pll, 0.0f);
}

void pll_reset(srf_pll_t *pll, float theta_rad)
{
    if (pll == 0) {
        return;
    }

    pi_reset(&pll->pi);
    pll->theta_rad = wrap_angle_rad(theta_rad);
    pll->omega_rad_s = pll->nominal_omega_rad_s;
    pll->lock_ticks = 0u;
    pll->locked = false;
}

float pll_step(srf_pll_t *pll, alphabeta_t voltage)
{
    if (pll == 0) {
        return 0.0f;
    }

    const dq_t vdq = park_transform(voltage, pll->theta_rad);
    const float magnitude = sqrtf((voltage.alpha * voltage.alpha) + (voltage.beta * voltage.beta));
    const float phase_error = vdq.q / (magnitude + 1.0e-6f);
    const float delta_omega = pi_step(&pll->pi, 0.0f, -phase_error);

    pll->omega_rad_s = pll->nominal_omega_rad_s + delta_omega;
    pll->theta_rad = wrap_angle_rad(pll->theta_rad + (pll->omega_rad_s * pll->pi.ts));

    if (fabsf(phase_error) < pll->lock_error_rad) {
        if (pll->lock_ticks < pll->lock_ticks_required) {
            pll->lock_ticks++;
        }
    } else {
        pll->lock_ticks = 0u;
    }
    pll->locked = pll->lock_ticks >= pll->lock_ticks_required;
    return pll->theta_rad;
}

