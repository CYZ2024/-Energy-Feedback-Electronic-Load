# CubeMX Port Template

Chinese version: [README_CubeMX_cn.md](README_CubeMX_cn.md)

This folder contains copy-ready templates for integrating the board-agnostic
firmware into a CubeMX-generated STM32 project.

The intended ownership split is:

- CubeMX owns peripheral initialization: clocks, GPIO, ADC, DMA, TIM/HRTIM.
- `firmware/math`, `firmware/control`, `firmware/core`, and `firmware/app` own
  reusable control logic.
- `Core/Src/efload_port.c` owns STM32-specific adaptation between CubeMX handles
  and the HAL contracts declared in `firmware/hal/include`.

## Files To Copy Into A CubeMX Project

Copy these files:

```text
firmware/platform/stm32_template/efload_port.h
    -> Core/Inc/efload_port.h

firmware/platform/stm32_template/efload_port.c
    -> Core/Src/efload_port.c

firmware/platform/stm32_template/board_template_stm32.h
    -> Core/Inc/board.h
```

Then add these include paths to the CubeMX project:

```text
firmware/core/include
firmware/math/include
firmware/control/include
firmware/app/include
firmware/hal/include
firmware/config
Core/Inc
```

Add these source folders/files to the MCU build:

```text
firmware/math/src/*.c
firmware/control/src/*.c
firmware/app/src/*.c
Core/Src/efload_port.c
```

Do not build these in the MCU firmware:

```text
firmware/platform/host/*
firmware/platform/stm32_stub/*
tests/*
docs/*
references/*
```

## CubeMX Peripheral Checklist

PWM:

- Use center-aligned PWM.
- Configure 20 kHz switching frequency.
- Enable complementary outputs for all bridge legs.
- Configure hardware deadtime before gate enable is possible.
- Configure break input and safe idle states.
- Keep gate-enable GPIO disabled by default.

ADC:

- Trigger sampling from the PWM timer or HRTIM.
- Sample near the quiet point of the PWM period.
- Use DMA or injected conversions according to latency needs.
- Convert ADC counts to SI units inside `efload_port.c`, not in core control.

Protection:

- Route fast over-current to timer/HRTIM break if hardware supports it.
- Read driver fault and emergency stop through `port_protection_read_latched()`.
- Do not automatically clear `FAULT`; require an explicit command.

Bring-up order:

1. Compile with all gate enables physically disconnected or disabled.
2. Verify PWM frequency, complementary outputs, deadtime, and break behavior.
3. Verify ADC conversion to V/A using known inputs.
4. Run CVCF alone at low DC voltage and current limit.
5. Run rectifier PLL with `id_ref = 0`.
6. Increase `id_ref` gradually from a small value, such as 0.1 A.
