# STM32 Porting Guide

This project intentionally does not include a CubeMX `.ioc` file. Add one only
after the STM32 part and development board are chosen.

## Porting Steps

1. Choose the STM32 part and board.
2. Copy `firmware/platform/stm32_template/board_template_stm32.h` to
   `Core/Inc/board.h` in the CubeMX project.
3. Fill ADC scale, offset, polarity, and phase-order fields.
4. Configure center-aligned PWM for both three-phase bridges.
5. Configure hardware deadtime and timer/HRTIM break input before enabling gates.
6. Trigger ADC sampling from the PWM timer at a low-noise point.
7. Implement or complete the HAL adapter in `Core/Src/efload_port.c`.
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

## CubeMX Template

Use `firmware/platform/stm32_template/README_CubeMX.md` as the starting point
when creating a CubeMX project.

The template provides:

- `efload_port.h`: public entry points for init/start/stop/control-period calls.
- `efload_port.c`: STM32 HAL adaptation skeleton for ADC, PWM, timer, GPIO, and
  protection callbacks.
- `board_template_stm32.h`: board-specific ADC indexes, scale factors, signs,
  timing, and protection thresholds.

Copy them into:

```text
Core/Inc/efload_port.h
Core/Src/efload_port.c
Core/Inc/board.h
```

Add the reusable firmware include paths and source files listed in
`README_CubeMX.md` to the CubeMX-generated project.

## Concrete Platform Folder Option

If you do not want to put the adapter in `Core/Src`, create a concrete platform
folder such as:

```text
firmware/platform/stm32_g474_eval/
|-- hal_adc_stm32.c
|-- hal_pwm_stm32.c
|-- hal_timer_stm32.c
|-- hal_gpio_stm32.c
|-- hal_protection_stm32.c
`-- board.h
```

Do not put STM32 HAL includes into `firmware/core`, `firmware/math`, or
`firmware/control`.
