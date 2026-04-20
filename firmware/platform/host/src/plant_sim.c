#include "efload_host/plant_sim.h"

#include <math.h>

void plant_sim_init(plant_sim_t *plant, float tau_s, float gain)
{
    if (plant == 0) {
        return;
    }

    plant->current_a.a = 0.0f;
    plant->current_a.b = 0.0f;
    plant->current_a.c = 0.0f;
    plant->voltage_v.a = 0.0f;
    plant->voltage_v.b = 0.0f;
    plant->voltage_v.c = 0.0f;
    plant->tau_s = tau_s;
    plant->gain = gain;
}

abc_t plant_sim_step_current(plant_sim_t *plant, abc_t command_v, float ts)
{
    if ((plant == 0) || (plant->tau_s <= 0.0f)) {
        abc_t zero = {0.0f, 0.0f, 0.0f};
        return zero;
    }

    const float alpha = ts / plant->tau_s;
    plant->current_a.a += alpha * ((command_v.a * plant->gain) - plant->current_a.a);
    plant->current_a.b += alpha * ((command_v.b * plant->gain) - plant->current_a.b);
    plant->current_a.c += alpha * ((command_v.c * plant->gain) - plant->current_a.c);
    plant->voltage_v = command_v;
    return plant->current_a;
}

float abc_rms(abc_t value)
{
    return sqrtf(((value.a * value.a) + (value.b * value.b) + (value.c * value.c)) / 3.0f);
}

