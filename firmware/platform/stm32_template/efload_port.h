#ifndef EFLOAD_PORT_H
#define EFLOAD_PORT_H

#include "efload_app/supervisor.h"

#ifdef __cplusplus
extern "C" {
#endif

extern supervisor_t g_efload_supervisor;

void efload_port_init(void);
void efload_port_start(void);
void efload_port_stop(void);
void efload_port_clear_fault(void);
void efload_port_on_control_period(void);

#ifdef __cplusplus
}
#endif

#endif

