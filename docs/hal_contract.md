# HAL Contract

The firmware core uses SI units and does not know ADC codes, timer registers,
GPIO ports, or STM32 HAL handles. Platform code owns all hardware conversion.

## Call Contexts

| Interface | Typical caller | Frequency | Context |
| --- | --- | --- | --- |
| ADC read | control-period task | 20 kHz | ISR or high-priority task |
| PWM update | control-period task | 20 kHz | ISR, after ADC sample is valid |
| PWM disable | fault path | immediate | ISR-safe, must be idempotent |
| Protection read | supervisor | 20 kHz or faster | ISR-safe |
| Timer read | supervisor/startup | as needed | ISR-safe |

## ADC

`hal_adc_frame_t` contains physical values only:

- phase voltages in V
- phase currents in A
- DC bus voltage in V

ADC offset, gain, polarity, channel order, and calibration are platform
responsibilities. For the expected 0 V to 3.3 V front end with 1.65 V midpoint:

\[
x_{physical}=(adc_v-1.65)\cdot gain
\]

The core never receives raw ADC counts.

## PWM

The control core outputs normalized duties in \([0,1]\). The platform converts
them to timer compare registers and must also enforce:

- complementary output polarity
- deadtime
- break input behavior
- safe idle state
- minimum and maximum compare limits

`hal_pwm_disable()` must be safe to call repeatedly and must shut down both
converter bridges.

## Protection

Hardware over-current and emergency stop paths must not depend on firmware
latency. The platform should route comparator/driver faults to timer break or
gate-driver shutdown where available. The HAL exposes latched status so the app
can record and hold `FAULT`.

## Errors

HAL functions return `bool` where a read/write can fail. A `false` return causes
the supervisor to enter `FAULT` and call PWM disable.

