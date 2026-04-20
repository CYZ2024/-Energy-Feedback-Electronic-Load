# STM32 Stub Port

This directory is intentionally not built by the host CMake target.

When a concrete STM32 board is selected, implement the HAL contracts from
`firmware/hal/include` in a new board-specific platform directory. Suggested
peripheral mapping:

- TIM1/TIM8 center-aligned complementary PWM, or HRTIM on STM32G4.
- Hardware deadtime and break input enabled before gate enable.
- ADC conversions triggered by the PWM timer at a quiet sampling point.
- DMA for regular ADC groups, injected conversions where latency matters.
- COMP/OPAMP and timer break for fast over-current shutdown.
- Driver fault and emergency stop routed to independent hardware shutdown.

Required implementation files for a real board:

- `hal_adc_stm32.c`
- `hal_pwm_stm32.c`
- `hal_timer_stm32.c`
- `hal_gpio_stm32.c`
- `hal_protection_stm32.c`
- board configuration with ADC scaling, phase order, PWM period, and polarities

