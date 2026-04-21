#ifndef EFLOAD_BOARD_H
#define EFLOAD_BOARD_H

/*
 * Copy this file to Core/Inc/board.h in a CubeMX project and replace all TODO
 * values with values from the selected board and analog front end.
 */

#include <stdint.h>

#define EFLOAD_BOARD_ADC_FULL_SCALE_COUNT (4095.0f)
#define EFLOAD_BOARD_ADC_REF_V (3.3f)
#define EFLOAD_BOARD_ADC_BIPOLAR_MID_V (1.65f)

/* Control timing. */
#define EFLOAD_BOARD_CONTROL_FREQUENCY_HZ (20000.0f)
#define EFLOAD_BOARD_PWM_DEADTIME_NS (1000u) /* TODO: verify from MOSFET driver timing. */

/* Protection thresholds. */
#define EFLOAD_BOARD_DC_BUS_MIN_V (40.0f)
#define EFLOAD_BOARD_DC_BUS_MAX_V (70.0f)
#define EFLOAD_BOARD_PHASE_CURRENT_LIMIT_A (3.0f)

/*
 * ADC channel indexes in the DMA buffer.
 * TODO: match the CubeMX ADC regular/injected conversion order.
 */
#define EFLOAD_ADC_IDX_CVCF_VA (0u)
#define EFLOAD_ADC_IDX_CVCF_VB (1u)
#define EFLOAD_ADC_IDX_CVCF_VC (2u)
#define EFLOAD_ADC_IDX_CVCF_IA (3u)
#define EFLOAD_ADC_IDX_CVCF_IB (4u)
#define EFLOAD_ADC_IDX_CVCF_IC (5u)
#define EFLOAD_ADC_IDX_RECT_VA (6u)
#define EFLOAD_ADC_IDX_RECT_VB (7u)
#define EFLOAD_ADC_IDX_RECT_VC (8u)
#define EFLOAD_ADC_IDX_RECT_IA (9u)
#define EFLOAD_ADC_IDX_RECT_IB (10u)
#define EFLOAD_ADC_IDX_RECT_IC (11u)
#define EFLOAD_ADC_IDX_DC_BUS (12u)
#define EFLOAD_ADC_CHANNEL_COUNT (13u)

/*
 * Analog scaling from ADC pin voltage to physical unit.
 * For bipolar signals:
 *     physical = (pin_v - offset_v) * gain
 * For DC bus:
 *     physical = pin_v * gain
 *
 * TODO: replace with real resistor-divider/current-sensor gains.
 */
#define EFLOAD_GAIN_CVCF_VA_V_PER_V (1.0f)
#define EFLOAD_GAIN_CVCF_VB_V_PER_V (1.0f)
#define EFLOAD_GAIN_CVCF_VC_V_PER_V (1.0f)
#define EFLOAD_GAIN_CVCF_IA_A_PER_V (1.0f)
#define EFLOAD_GAIN_CVCF_IB_A_PER_V (1.0f)
#define EFLOAD_GAIN_CVCF_IC_A_PER_V (1.0f)

#define EFLOAD_GAIN_RECT_VA_V_PER_V (1.0f)
#define EFLOAD_GAIN_RECT_VB_V_PER_V (1.0f)
#define EFLOAD_GAIN_RECT_VC_V_PER_V (1.0f)
#define EFLOAD_GAIN_RECT_IA_A_PER_V (1.0f)
#define EFLOAD_GAIN_RECT_IB_A_PER_V (1.0f)
#define EFLOAD_GAIN_RECT_IC_A_PER_V (1.0f)

#define EFLOAD_GAIN_DC_BUS_V_PER_V (20.0f)

/*
 * Optional polarity corrections. Set to -1.0f when a channel is wired with the
 * opposite sign from the control convention.
 */
#define EFLOAD_SIGN_CVCF_VA (1.0f)
#define EFLOAD_SIGN_CVCF_VB (1.0f)
#define EFLOAD_SIGN_CVCF_VC (1.0f)
#define EFLOAD_SIGN_CVCF_IA (1.0f)
#define EFLOAD_SIGN_CVCF_IB (1.0f)
#define EFLOAD_SIGN_CVCF_IC (1.0f)
#define EFLOAD_SIGN_RECT_VA (1.0f)
#define EFLOAD_SIGN_RECT_VB (1.0f)
#define EFLOAD_SIGN_RECT_VC (1.0f)
#define EFLOAD_SIGN_RECT_IA (1.0f)
#define EFLOAD_SIGN_RECT_IB (1.0f)
#define EFLOAD_SIGN_RECT_IC (1.0f)

#endif

