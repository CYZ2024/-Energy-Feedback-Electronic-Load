#ifndef EFLOAD_HAL_GPIO_H
#define EFLOAD_HAL_GPIO_H

#include <stdbool.h>

typedef struct {
    void (*set_status_led)(bool on, void *context);
    bool (*read_estop)(void *context);
    void *context;
} hal_gpio_t;

#endif

