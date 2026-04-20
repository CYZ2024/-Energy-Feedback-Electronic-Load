#ifndef EFLOAD_HAL_PROTECTION_H
#define EFLOAD_HAL_PROTECTION_H

#include <stdbool.h>
#include <stdint.h>

enum {
    HAL_PROTECTION_NONE = 0u,
    HAL_PROTECTION_OVERCURRENT = 1u << 0,
    HAL_PROTECTION_OVERVOLTAGE = 1u << 1,
    HAL_PROTECTION_UNDERVOLTAGE = 1u << 2,
    HAL_PROTECTION_DRIVER_FAULT = 1u << 3,
    HAL_PROTECTION_ESTOP = 1u << 4
};

typedef struct {
    uint32_t (*read_latched)(void *context);
    void (*clear_latched)(void *context);
    void *context;
} hal_protection_t;

#endif

