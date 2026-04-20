# Energy Feedback Electronic Load Firmware

Board-agnostic software skeleton for a back-to-back dual-PWM energy-feedback
electronic load.

The target MCU family is STM32, but the exact part and development board are
not fixed yet. For that reason the reusable control code is pure C, uses
`float` with SI units, and has no dependency on STM32 HAL, LL, CMSIS, CubeMX, or
pin names.

## Build And Test

```powershell
cmake -S . -B build-host -G Ninja -DBUILD_HOST_TESTS=ON
cmake --build build-host
ctest --test-dir build-host --output-on-failure
```

Hardware header guard check:

```powershell
rg -i "stm32|cmsis|\bHAL_|\bLL_" firmware/core firmware/control firmware/math
```

The command should print no matches.

## Layout

- `docs/`: design notes, HAL contract, and porting guide.
- `references/`: original PDF source material.
- `firmware/core/`: shared data types and status definitions.
- `firmware/math/`: pure math/control primitives.
- `firmware/control/`: CVCF source and CC rectifier control loops.
- `firmware/app/`: startup, supervision, and fault-latching state machine.
- `firmware/hal/`: hardware abstraction headers only.
- `firmware/platform/host/`: mocks and light plant simulation for tests.
- `firmware/platform/stm32_stub/`: STM32 porting placeholders.
- `tests/`: host unit, integration, and safety tests.

## Adding A Board

1. Copy `firmware/config/board_template.h` to a board-specific header outside
   version control or to a named board package.
2. Implement the HAL interfaces in `firmware/hal/include`.
3. Convert ADC counts to V/A in the platform layer only.
4. Keep PWM deadtime, break input, and comparator shutdown active in hardware.
5. Bring up low-voltage PWM and sampling before enabling closed-loop power flow.
