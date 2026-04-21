# Energy Feedback Electronic Load Firmware

Chinese version: [README_cn.md](README_cn.md)

Board-agnostic firmware skeleton for a back-to-back dual-PWM energy-feedback
electronic load.

The project targets the STM32 family, but it does not bind to a concrete MCU,
development board, CubeMX `.ioc`, pin map, or HAL handle yet. CubeMX should own
peripheral initialization; this repository owns reusable control algorithms,
application supervision, HAL contracts, host tests, and STM32 porting templates.

## Current Status

Implemented:

- C99 reusable control core.
- Clarke/Park transforms, Tustin PI, SRF-PLL, SVPWM, ramp, and first-order LPF.
- CVCF inverter-source control framework.
- Constant-current active-rectifier/electronic-load control framework.
- `INIT -> PRECHARGE -> SYNC -> RUN -> FAULT` supervision state machine.
- HAL contracts for ADC, PWM, timer, GPIO, and protection.
- Host mocks, light plant simulation, and CTest tests.
- Copy-ready CubeMX port template under `firmware/platform/stm32_template/`.

Not included yet:

- Concrete STM32 CubeMX project or `.ioc` file.
- Board pin mapping, ADC channel order, DMA setup, and timer/HRTIM setup.
- Hardware-tuned PI gains.
- Final production protection thresholds and calibration data.

## Build And Test On Host

Requirements:

- CMake 3.20 or newer
- Ninja
- C compiler
- ripgrep, optional, for boundary checks

Commands:

```powershell
cmake -S . -B build-host -G Ninja -DBUILD_HOST_TESTS=ON
cmake --build build-host
ctest --test-dir build-host --output-on-failure
```

Check that reusable core/control/math layers do not depend on STM32 HAL, LL, or
CMSIS:

```powershell
rg -i "stm32|cmsis|\bHAL_|\bLL_" firmware/core firmware/control firmware/math
```

Expected result: no matches.

## Project Layout

```text
.
|-- cmake/                         CMake helper modules
|-- docs/                          design notes, HAL contract, porting guide
|-- firmware/
|   |-- app/                       startup, supervision, fault latch
|   |-- config/                    default config and board templates
|   |-- control/                   CVCF and active-rectifier control loops
|   |-- core/                      shared data types and status definitions
|   |-- hal/                       hardware abstraction headers
|   |-- math/                      pure math/control primitives
|   `-- platform/
|       |-- host/                  host mocks and light simulation
|       |-- stm32_stub/            non-built STM32 placeholder
|       `-- stm32_template/        CubeMX copy-ready port template
|-- references/                    original PDF source material
|-- tests/                         unit, integration, and safety tests
|-- CMakeLists.txt
`-- README.md
```

## CubeMX Migration

Start from:

```text
firmware/platform/stm32_template/README_CubeMX.md
```

Copy these files into a CubeMX project:

```text
firmware/platform/stm32_template/efload_port.h
    -> Core/Inc/efload_port.h

firmware/platform/stm32_template/efload_port.c
    -> Core/Src/efload_port.c

firmware/platform/stm32_template/board_template_stm32.h
    -> Core/Inc/board.h
```

Add these include paths to the CubeMX project:

```text
firmware/core/include
firmware/math/include
firmware/control/include
firmware/app/include
firmware/hal/include
firmware/config
Core/Inc
```

Compile these reusable firmware sources in the MCU project:

```text
firmware/math/src/*.c
firmware/control/src/*.c
firmware/app/src/*.c
Core/Src/efload_port.c
```

Do not compile these into the MCU firmware:

```text
firmware/platform/host/*
firmware/platform/stm32_stub/*
tests/*
docs/*
references/*
```

CubeMX should configure clocks, GPIO, ADC, DMA, TIM/HRTIM, complementary PWM,
deadtime, and break inputs. `efload_port.c` should convert CubeMX handles and
ADC DMA buffers into the HAL contracts declared under `firmware/hal/include`.

## Bring-Up Order

1. Build the CubeMX project with gate enables physically disconnected or forced
   off.
2. Verify PWM frequency, complementary outputs, deadtime, idle state, and break
   input on an oscilloscope.
3. Verify ADC conversion to physical V/A values with known inputs.
4. Run CVCF alone at low DC voltage and current limit.
5. Run rectifier PLL with `id_ref = 0`.
6. Increase rectifier current command from a small value, such as 0.1 A.
7. Approach the 2 A design target only after protection and power direction are
   verified.

## Safety Notes

This repository is a software skeleton and verification harness, not a
ready-to-connect power-stage firmware image.

Before real power-stage testing, confirm:

- Hardware over-current shutdown does not depend on software latency.
- PWM deadtime and complementary output polarity are verified by oscilloscope.
- DC-bus undervoltage, overvoltage, and precharge behavior are reliable.
- ADC scale, offset, phase order, and polarity are calibrated.
- Fault recovery requires an explicit clear command and never happens
  automatically from `RUN`.

## Documents

- `docs/energy_feedback_load_design.md`
- `docs/hal_contract.md`
- `docs/porting_guide.md`
- `firmware/platform/stm32_template/README_CubeMX.md`

## License

No license has been declared yet. Add a `LICENSE` file before public reuse or
distribution.
