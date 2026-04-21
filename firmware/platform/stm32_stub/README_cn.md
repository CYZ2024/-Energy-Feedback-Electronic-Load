# STM32 占位移植目录

English version: [README.md](README.md)

这个目录不会参与主机端 CMake 构建。

当具体 STM32 芯片和开发板确定后，请在新的板级平台目录中实现 `firmware/hal/include` 里的 HAL 契约。建议外设映射如下：

- STM32G4 可优先使用 HRTIM；也可以用 TIM1/TIM8 做中心对齐互补 PWM。
- 在允许门极使能前，必须先配置硬件死区和 break 输入。
- ADC 由 PWM 定时器或 HRTIM 在低噪声采样点触发。
- 常规组可使用 DMA；对延迟敏感的采样可考虑注入组。
- 使用 COMP/OPAMP 和定时器 break 做快速过流关断。
- 驱动故障和急停输入应接入独立硬件关断路径。

真实开发板需要实现的文件通常包括：

```text
hal_adc_stm32.c
hal_pwm_stm32.c
hal_timer_stm32.c
hal_gpio_stm32.c
hal_protection_stm32.c
board.h
```

注意：不要把 STM32 HAL、LL、CMSIS 头文件包含进 `firmware/core`、`firmware/math` 或 `firmware/control`。
