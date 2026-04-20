#ifndef EFLOAD_HOST_PLANT_SIM_H
#define EFLOAD_HOST_PLANT_SIM_H

#include "efload_core/types.h"

typedef struct {
    abc_t current_a;
    abc_t voltage_v;
    float tau_s;
    float gain;
} plant_sim_t;

void plant_sim_init(plant_sim_t *plant, float tau_s, float gain);
abc_t plant_sim_step_current(plant_sim_t *plant, abc_t command_v, float ts);
float abc_rms(abc_t value);

#endif

