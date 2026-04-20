#ifndef EFLOAD_APP_STARTUP_SEQUENCE_H
#define EFLOAD_APP_STARTUP_SEQUENCE_H

#include <stdbool.h>
#include <stdint.h>

bool startup_delay_elapsed(uint32_t now_us, uint32_t entered_us, uint32_t delay_us);

#endif

