#ifndef EFLOAD_MATH_CLARKE_PARK_H
#define EFLOAD_MATH_CLARKE_PARK_H

#include "efload_core/types.h"

/*
 * Amplitude-invariant Clarke transform for balanced three-phase quantities.
 * A d-axis value of 1.0 maps to three phase sinusoids with peak amplitude 1.0.
 */
alphabeta_t clarke_amp_invariant(abc_t abc);
abc_t inverse_clarke_amp_invariant(alphabeta_t alphabeta);
dq_t park_transform(alphabeta_t alphabeta, float theta_rad);
alphabeta_t inverse_park_transform(dq_t dq, float theta_rad);
float wrap_angle_rad(float angle_rad);
float clamp_f32(float value, float min_value, float max_value);

#endif

