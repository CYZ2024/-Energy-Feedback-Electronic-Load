# 能量回馈电子负载固件

面向背靠背双 PWM 变换器的能量回馈电子负载固件骨架。

本项目的目标是把传统耗能型交流负载测试装置，整理成一个可在 PC 端先验证控制算法、后续再移植到 STM32 硬件平台的嵌入式固件工程。当前代码重点放在可复用控制核心、HAL 接口边界、主机端单元测试和 STM32 移植占位上。

## 项目状态

当前阶段：板级无关固件骨架。

已完成：

- 纯 C 控制核心，遵循 C99。
- Clarke/Park 变换、PI、PLL、SVPWM、斜坡和低通滤波等基础模块。
- CVCF 逆变源控制框架。
- 恒流型有源整流/电子负载控制框架。
- 启停、监督和故障锁存状态机。
- HAL 头文件契约，隔离 ADC、PWM、GPIO、保护和时基。
- 主机端 mock 平台、轻量被控对象仿真和测试用例。

尚未包含：

- 具体 STM32 CubeMX 工程或 `.ioc` 文件。
- 具体开发板的 ADC/PWM/DMA/引脚配置。
- 真实硬件上的 PI 参数整定结果。
- 最终硬件保护阈值和生产级校准数据。

## 设计目标

系统采用两组三相全桥变换器：

- 变换器 1：作为三相 CVCF 逆变源，输出稳定交流电压。
- 变换器 2：作为有源电子负载/整流器，从交流侧吸收电流并把能量送回直流母线。
- 直流电源：主要补偿变换器、滤波器和线路损耗，而不是承担全部负载功率。

软件设计原则：

- 控制核心只使用 `float` 和 SI 单位。
- 核心层不依赖 STM32 HAL、LL、CMSIS、CubeMX 或具体引脚名。
- ADC 原始码值、采样比例、零偏、相序和极性修正全部放在平台层。
- PWM 死区、刹车输入、比较器关断和驱动保护优先由硬件实现。
- 在真实功率级闭环前，先用主机端测试和低压硬件测试逐步验证。

## 技术栈

- 语言：C99
- 构建系统：CMake
- 推荐生成器：Ninja
- 测试：CTest + 主机端 C 测试程序
- 目标平台：STM32 系列 MCU，具体型号待定
- 当前验证平台：Windows/PC 主机构建

## 快速开始

### 1. 准备工具

需要安装：

- CMake 3.20 或更高版本
- Ninja
- 一个可用的 C 编译器
- ripgrep，可选，用于边界检查

Windows 上可以在 PowerShell 中执行下面命令。

### 2. 构建主机端测试

```powershell
cmake -S . -B build-host -G Ninja -DBUILD_HOST_TESTS=ON
cmake --build build-host
```

### 3. 运行测试

```powershell
ctest --test-dir build-host --output-on-failure
```

### 4. 检查控制核心是否误依赖硬件 SDK

```powershell
rg -i "stm32|cmsis|\bHAL_|\bLL_" firmware/core firmware/control firmware/math
```

理想结果是不输出任何匹配项。

## 项目结构

```text
.
├── cmake/                         CMake 辅助模块
├── docs/                          设计说明、HAL 契约和移植说明
├── firmware/
│   ├── app/                       启停、监督和故障锁存状态机
│   ├── config/                    默认配置和板级配置模板
│   ├── control/                   CVCF 与恒流整流控制环
│   ├── core/                      公共数据类型和状态定义
│   ├── hal/                       硬件抽象层头文件契约
│   ├── math/                      控制数学基础模块
│   └── platform/
│       ├── host/                  主机端 mock 和轻量仿真
│       └── stm32_stub/            STM32 移植占位
├── references/                    原始参考资料
├── tests/                         单元、集成和安全测试
├── CMakeLists.txt                 顶层构建入口
└── README.md
```

## 核心模块

| 模块 | 路径 | 说明 |
| --- | --- | --- |
| Clarke/Park 变换 | `firmware/math/src/clarke_park.c` | 三相静止坐标与 dq 坐标变换 |
| PI 控制器 | `firmware/math/src/pi.c` | 限幅、抗积分饱和和离散积分 |
| PLL | `firmware/math/src/pll.c` | 用于锁定交流电压相位 |
| SVPWM | `firmware/math/src/svpwm.c` | 归一化三相 PWM 占空比生成 |
| 斜坡 | `firmware/math/src/ramp.c` | 软启动和给定值平滑 |
| 低通滤波 | `firmware/math/src/lpf.c` | 一阶滤波工具 |
| dq 电流环 | `firmware/control/src/dq_current_loop.c` | dq 坐标下的电流闭环 |
| CVCF 源 | `firmware/control/src/cvcf_source.c` | 变换器 1 控制框架 |
| CC 整流负载 | `firmware/control/src/cc_rectifier.c` | 变换器 2 控制框架 |
| 监督器 | `firmware/app/src/supervisor.c` | 状态机、保护判定和故障锁存 |

## 测试覆盖

当前测试分为三类：

- `tests/unit/`：数学模块和边界条件测试。
- `tests/integration/`：闭环控制框架的主机端集成测试。
- `tests/safety/`：故障锁存和保护路径测试。

已接入的测试目标包括：

- `test_clarke_park`
- `test_pi`
- `test_pll`
- `test_svpwm`
- `test_cc_closed_loop`
- `test_cvcf_closed_loop`
- `test_fault_latch`

## HAL 边界

控制核心不直接接触寄存器、ADC 码值或定时器比较寄存器。平台层负责把硬件细节转换成核心能理解的物理量。

HAL 接口约定：

- ADC 输入使用 V、A 等物理单位。
- PWM 输出使用 `[0, 1]` 归一化占空比。
- `hal_pwm_disable()` 必须可重复调用，并能从任意状态关闭两组三相桥。
- 过流、急停和驱动故障应优先走硬件比较器、定时器刹车输入或门极驱动关断。
- HAL 读写失败会触发监督器进入 `FAULT`，并关闭 PWM。

详细说明见 [docs/hal_contract.md](docs/hal_contract.md)。

## STM32 移植路线

项目当前没有绑定具体 STM32 型号。后续移植建议按以下顺序推进：

1. 选定 STM32 型号和开发板。
2. 复制 `firmware/config/board_template.h`，建立具体板级配置。
3. 配置两组三相互补 PWM，先确认死区和安全空闲态。
4. 配置 ADC 由 PWM 定时器在低噪声点触发采样。
5. 实现 `firmware/hal/include` 下声明的 HAL 接口。
6. 确认故障输入、比较器、刹车输入和驱动关断路径。
7. 在门极使能断开或功率级低压条件下做开环验证。
8. 逐步进入 ADC 校准、开环 PWM、单变换器闭环、双变换器低电流闭环。

详细说明见 [docs/porting_guide.md](docs/porting_guide.md)。

## 安全说明

本项目涉及功率电子、PWM 驱动、直流母线和能量回馈路径。当前仓库是软件骨架和测试验证工程，不是可直接连接真实功率级运行的完整产品固件。

在接入真实硬件前，请至少确认：

- 硬件过流保护不依赖软件响应时间。
- PWM 死区和互补输出极性已经用示波器确认。
- 直流母线欠压、过压和预充逻辑可靠。
- ADC 采样比例、零偏、相序和极性已经校准。
- 低压、限流、小电流条件下验证功率方向。
- 故障后必须人工清除，不自动恢复运行。

## 相关文档

- [能量回馈电子负载设计方案](docs/energy_feedback_load_design.md)
- [HAL Contract](docs/hal_contract.md)
- [STM32 Porting Guide](docs/porting_guide.md)

## 后续计划

- 选定首个 STM32 目标板。
- 补充具体板级配置和 HAL 实现。
- 增加 ADC 标定、相序检查和 PWM 极性自检流程。
- 在低压硬件平台上记录第一组闭环 PI 参数。
- 补充 GitHub Issues、里程碑和项目看板，用于跟踪硬件移植、算法验证和测试任务。

## 许可证

当前仓库尚未声明许可证。若计划公开发布或允许他人复用，建议补充 `LICENSE` 文件。
