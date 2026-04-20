#include "efload_app/supervisor.h"

void converter_tasks_on_pwm_period(supervisor_t *supervisor)
{
    supervisor_on_pwm_period(supervisor);
}

