#ifndef EFLOAD_HAL_TIMER_H
#define EFLOAD_HAL_TIMER_H

#include <stdint.h>

typedef struct {
    uint32_t (*now_us)(void *context);
    void *context;
} hal_timer_t;

#endif

