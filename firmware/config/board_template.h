#ifndef EFLOAD_BOARD_TEMPLATE_H
#define EFLOAD_BOARD_TEMPLATE_H

/*
 * Copy this file into a concrete board package and rename it to board.h.
 * Keep ADC counts, timer counts, GPIO names, and pin polarity out of the core.
 */

#define BOARD_TODO_ADC_CURRENT_GAIN_A_PER_V (0.0f)
#define BOARD_TODO_ADC_VOLTAGE_GAIN_V_PER_V (0.0f)
#define BOARD_TODO_ADC_MIDPOINT_V (1.65f)
#define BOARD_TODO_PWM_PERIOD_COUNTS (0u)
#define BOARD_TODO_PWM_DEADTIME_NS (0u)
#define BOARD_TODO_PHASE_ORDER_A_B_C (1)
#define BOARD_TODO_GATE_ENABLE_ACTIVE_HIGH (1)
#define BOARD_TODO_FAULT_ACTIVE_HIGH (1)

#endif

