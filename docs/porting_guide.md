# STM32 Porting Guide

This project intentionally does not include a CubeMX `.ioc` file. Add one only
after the STM32 part and development board are chosen.

## Porting Steps

1. Choose the STM32 part and board.
2. Copy `firmware/config/board_template.h` into a concrete board package.
3. Fill ADC scale, offset, polarity, and phase-order fields.
4. Configure center-aligned PWM for both three-phase bridges.
5. Configure hardware deadtime and timer break input before enabling gates.
6. Trigger ADC sampling from the PWM timer at a low-noise point.
7. Implement the HAL interfaces declared under `firmware/hal/include`.
8. Confirm `hal_pwm_disable()` disables both bridges from any state.
9. Run first tests with gate enable disconnected or disabled.
10. Bring up low-voltage PWM, then ADC calibration, then open-loop, then closed-loop.

## Suggested STM32 Peripheral Mapping

For an STM32G4-class part:

- TIM1/TIM8 or HRTIM for complementary PWM.
- ADC injected or regular conversions triggered by PWM timer events.
- DMA for sample transfer if using regular conversions.
- COMP/OPAMP for fast over-current protection where available.
- GPIO or driver fault pin for gate-driver status.

## Files To Implement Later

Create a concrete platform folder such as:

```text
firmware/platform/stm32_g474_eval/
├─ hal_adc_stm32.c
├─ hal_pwm_stm32.c
├─ hal_timer_stm32.c
├─ hal_gpio_stm32.c
├─ hal_protection_stm32.c
└─ board.h
```

Do not put STM32 HAL includes into `firmware/core`, `firmware/math`, or
`firmware/control`.

