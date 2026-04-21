# 能量回馈电子负载固件

English version: [README.md](README.md)

这是一个面向背靠背双 PWM 能量回馈电子负载的板级解耦固件骨架。

项目目标芯片族是 STM32，但当前不绑定具体 MCU、开发板、CubeMX `.ioc`、引脚映射或 HAL 句柄。CubeMX 负责外设初始化；本仓库负责可复用控制算法、应用监督、HAL 契约、主机端测试和 STM32 移植模板。

## 当前状态

已实现：

- C99 可复用控制核心。
- Clarke/Park 变换、Tustin PI、SRF-PLL、SVPWM、斜坡和一阶低通滤波。
- CVCF 恒压恒频逆变源控制框架。
- 恒流有源整流/电子负载控制框架。
- `INIT -> PRECHARGE -> SYNC -> RUN -> FAULT` 监督状态机。
- ADC、PWM、定时器、GPIO 和保护接口的 HAL 契约。
- 主机端 mock、轻量仿真和 CTest 测试。
- CubeMX 移植模板：`firmware/platform/stm32_template/`。

暂未包含：

- 具体 STM32 CubeMX 工程或 `.ioc` 文件。
- 开发板引脚映射、ADC 通道顺序、DMA 配置、TIM/HRTIM 配置。
- 根据真实硬件整定后的 PI 参数。
- 最终生产级保护阈值和校准数据。

## 主机端构建和测试

需要：

- CMake 3.20 或更高版本
- Ninja
- C 编译器
- ripgrep，可选，用于边界检查

命令：

```powershell
cmake -S . -B build-host -G Ninja -DBUILD_HOST_TESTS=ON
cmake --build build-host
ctest --test-dir build-host --output-on-failure
```

检查可复用核心层、控制层、数学层是否误依赖 STM32 HAL、LL 或 CMSIS：

```powershell
rg -i "stm32|cmsis|\bHAL_|\bLL_" firmware/core firmware/control firmware/math
```

理想结果：无任何匹配输出。

## 项目结构

```text
.
|-- cmake/                         CMake 辅助模块
|-- docs/                          设计说明、HAL 契约、移植指南
|-- firmware/
|   |-- app/                       启动、监督、故障锁存
|   |-- config/                    默认配置和板级模板
|   |-- control/                   CVCF 与有源整流控制环
|   |-- core/                      公共数据类型和状态定义
|   |-- hal/                       硬件抽象头文件
|   |-- math/                      纯数学/控制基础模块
|   `-- platform/
|       |-- host/                  主机端 mock 和轻量仿真
|       |-- stm32_stub/            不参与构建的 STM32 占位说明
|       `-- stm32_template/        可复制到 CubeMX 的移植模板
|-- references/                    原始 PDF 参考资料
|-- tests/                         单元、集成和安全测试
|-- CMakeLists.txt
`-- README.md
```

## CubeMX 移植

从这里开始：

```text
firmware/platform/stm32_template/README_CubeMX.md
```

把这些文件复制到 CubeMX 工程：

```text
firmware/platform/stm32_template/efload_port.h
    -> Core/Inc/efload_port.h

firmware/platform/stm32_template/efload_port.c
    -> Core/Src/efload_port.c

firmware/platform/stm32_template/board_template_stm32.h
    -> Core/Inc/board.h
```

在 CubeMX 工程中加入这些 include path：

```text
firmware/core/include
firmware/math/include
firmware/control/include
firmware/app/include
firmware/hal/include
firmware/config
Core/Inc
```

在 MCU 工程中编译这些可复用源码：

```text
firmware/math/src/*.c
firmware/control/src/*.c
firmware/app/src/*.c
Core/Src/efload_port.c
```

不要编译这些目录：

```text
firmware/platform/host/*
firmware/platform/stm32_stub/*
tests/*
docs/*
references/*
```

CubeMX 应负责配置时钟、GPIO、ADC、DMA、TIM/HRTIM、互补 PWM、死区和 break 输入。`efload_port.c` 负责把 CubeMX 的句柄和 ADC DMA 缓冲区转换为 `firmware/hal/include` 下声明的 HAL 契约。

## 上板调试顺序

1. 在门极使能断开或强制关闭的条件下构建 CubeMX 工程。
2. 用示波器验证 PWM 频率、互补输出、死区、空闲状态和 break 输入。
3. 用已知输入验证 ADC 到 V/A 物理量的转换。
4. 在低直流电压和限流条件下单独运行 CVCF。
5. 让整流侧 PLL 在 `id_ref = 0` 时锁相。
6. 从很小的整流电流指令开始，例如 0.1 A。
7. 只有在保护和功率方向都确认后，再逐步接近 2 A 设计目标。

## 安全说明

本仓库是软件骨架和验证框架，不是可以直接连接真实功率级的完整固件镜像。

真实功率级测试前至少确认：

- 硬件过流关断不依赖软件响应时间。
- PWM 死区和互补输出极性已经用示波器验证。
- 直流母线欠压、过压和预充行为可靠。
- ADC 比例、零偏、相序和极性已经校准。
- 故障恢复需要显式清除命令，不能从 `RUN` 自动恢复。

## 文档

- `docs/energy_feedback_load_design.md`
- `docs/hal_contract.md`
- `docs/porting_guide.md`
- `firmware/platform/stm32_template/README_CubeMX.md`

## 许可证

当前尚未声明许可证。公开复用或分发前建议添加 `LICENSE` 文件。
