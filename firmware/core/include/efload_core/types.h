#ifndef EFLOAD_CORE_TYPES_H
#define EFLOAD_CORE_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float a;
    float b;
    float c;
} abc_t;

typedef struct {
    float alpha;
    float beta;
} alphabeta_t;

typedef struct {
    float d;
    float q;
} dq_t;

typedef struct {
    float a;
    float b;
    float c;
} duty_abc_t;

typedef struct {
    float min;
    float max;
} limits_f32_t;

typedef enum {
    CTRL_OK = 0,
    CTRL_LIMITED,
    CTRL_FAULT_OC,
    CTRL_FAULT_OV,
    CTRL_FAULT_UV,
    CTRL_FAULT_PLL_LOSS,
    CTRL_FAULT_HAL,
    CTRL_FAULT_INVALID
} ctrl_status_t;

typedef enum {
    CONVERTER_CVCF_SOURCE = 0,
    CONVERTER_CC_RECTIFIER = 1
} converter_id_t;

typedef struct {
    duty_abc_t cvcf;
    duty_abc_t rectifier;
    bool cvcf_enable;
    bool rectifier_enable;
    ctrl_status_t status;
} pwm_command_t;

#endif

