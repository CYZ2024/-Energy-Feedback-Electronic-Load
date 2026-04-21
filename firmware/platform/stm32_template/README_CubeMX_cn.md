# CubeMX 移植模板

English version: [README_CubeMX.md](README_CubeMX.md)

本目录提供可直接复制的模板，用于把板级解耦固件接入 CubeMX 生成的 STM32 工程。

推荐职责划分如下：

- CubeMX 负责外设初始化：时钟、GPIO、ADC、DMA、TIM/HRTIM。
- `firmware/math`、`firmware/control`、`firmware/core` 和 `firmware/app` 负责可复用控制逻辑。
- `Core/Src/efload_port.c` 负责把 CubeMX 生成的句柄适配到 `firmware/hal/include` 中声明的 HAL 契约。

## 复制到 CubeMX 工程的文件

复制这些文件：

```text
firmware/platform/stm32_template/efload_port.h
    -> Core/Inc/efload_port.h

firmware/platform/stm32_template/efload_port.c
    -> Core/Src/efload_port.c

firmware/platform/stm32_template/board_template_stm32.h
    -> Core/Inc/board.h
```

然后在 CubeMX 工程里加入这些 include path：

```text
firmware/core/include
firmware/math/include
firmware/control/include
firmware/app/include
firmware/hal/include
firmware/config
Core/Inc
```

把这些源文件加入 MCU 构建：

```text
firmware/math/src/*.c
firmware/control/src/*.c
firmware/app/src/*.c
Core/Src/efload_port.c
```

不要把这些目录编进 MCU 固件：

```text
firmware/platform/host/*
firmware/platform/stm32_stub/*
tests/*
docs/*
references/*
```

## CubeMX 外设检查清单

PWM：

- 使用中心对齐 PWM。
- 开关频率配置为 20 kHz。
- 两组三相桥所有桥臂启用互补输出。
- 在门极可能使能之前配置硬件死区。
- 配置 break 输入和安全空闲状态。
- gate-enable GPIO 默认保持关闭。

ADC：

- ADC 采样由 PWM 定时器或 HRTIM 触发。
- 采样点应避开开关噪声较大的瞬间。
- 根据延迟要求选择 DMA 或注入转换。
- ADC 码值到 SI 单位的转换只写在 `efload_port.c` 中，不写进控制核心。

保护：

- 若硬件支持，快速过流应接到定时器/HRTIM 的 break 输入。
- 通过 `port_protection_read_latched()` 读取驱动故障和急停状态。
- 不要自动清除 `FAULT`；必须由显式命令清除。

上板顺序：

1. 在门极使能断开或关闭的条件下编译并运行。
2. 验证 PWM 频率、互补输出、死区和 break 行为。
3. 用已知输入验证 ADC 到 V/A 的物理量转换。
4. 在低压条件下单独运行 CVCF。
5. 让整流侧 PLL 在 `id_ref = 0` 时锁相。
6. 从很小电流指令开始，例如 0.1 A。
