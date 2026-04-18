# ESP32 1kW MPPT Solar Charge Controller (Improved)

Firmware for a high-power DIY solar charge controller using ESP32, ADS1015, and a buck stage with MPPT/CV behavior.

Reference project (inspiration):
- https://www.instructables.com/DIY-1kW-MPPT-Solar-Charge-Controller/

This fork focuses on cleaner structure, practical protection logic, easier tuning, and smoother PlatformIO development.

## At A Glance

| Area | Details |
|---|---|
| MCU | ESP32 (PlatformIO target: featheresp32) |
| Control | PWM buck control with MPPT mode + non-MPPT regulated mode |
| Sensing | ADS1015 for voltage/current + NTC temperature input |
| UI | Auto-detected UI: 0.96in I2C OLED + rotary encoder (preferred) or 16x2 I2C LCD + buttons |
| Telemetry | Serial telemetry + optional Blynk cloud telemetry |
| Persistence | EEPROM-based settings load/save |
| Safety | Overcurrent, overvoltage, undervoltage, overtemperature, battery/source checks |

## Quick Start

### 1. Requirements
- VS Code with PlatformIO extension (or PlatformIO Core CLI)
- ESP32 board connected by USB
- Power stage disconnected from high-power source for first boot tests

### 2. Build
```bash
pio run
```

### 3. Upload
```bash
pio run -t upload
```

### 4. Monitor
```bash
pio device monitor -b 500000
```

### 5. First Boot Checks
- Confirm LCD boots and shows firmware info
- Verify voltage readings are stable and realistic
- Verify temperature is reasonable at room conditions
- Confirm PWM starts low and no protection flags are active

## Feature Overview

### Control and Charging
- MPPT algorithm path with perturb/observe-style behavior
- Alternate non-MPPT regulated mode
- Predictive PWM initialization to speed startup behavior
- PWM duty limiting based on configured max duty cycle

### Protection and Reliability
- Input undervoltage detection (IUV)
- Input overcurrent detection (IOC)
- Output overvoltage detection (OOV)
- Output overcurrent detection (OOC)
- Overtemperature detection (OTE)
- Flat voltage and battery-not-connected logic
- Backflow MOSFET control to reduce reverse flow risk

### User Experience
- Auto UI selection on boot:
	- OLED mode (SSD1306 128x64): rotary-encoder status and settings menu
	- LCD mode (16x2): legacy button-driven menu
- In OLED settings mode, converter switching is paused and output path is forced off for safe editing
- Settings are applied and saved when exiting the settings menu
- Persistent user settings in EEPROM
- Runtime metrics: power, Wh/kWh/MWh, SOC estimate, loop time
- Optional Blynk integration when library is present
- Adaptive encoder editing: fine increments at slow rotation and accelerated increments at high rotation speed

## Firmware Architecture

Dual-core split:
- Core 1 loop: sensors, protection, charging algorithm, onboard telemetry, OLED/LCD UI service
- Core 0 task: Wi-Fi setup and wireless telemetry service

## Project Layout

```text
.
|-- platformio.ini
|-- src/
|   |-- main.cpp
|   |-- charging.cpp/.h
|   |-- protection.cpp/.h
|   |-- sensors.cpp/.h
|   |-- telemetry.cpp/.h
|   |-- lcd.cpp/.h
|   |-- system.cpp/.h
|   |-- globals.cpp
|   `-- config.h
|-- include/
|-- lib/
`-- test/
```

## Pin Mapping

| Signal | GPIO |
|---|---|
| backflow_MOSFET | 27 |
| buck_IN (PWM) | 33 |
| buck_EN | 32 |
| LED | 2 |
| FAN | 16 |
| ADC_ALERT | 34 |
| TempSensor | 35 |
| buttonLeft | 18 |
| buttonRight | 17 |
| buttonBack | 19 |
| buttonSelect | 23 |
| encoderPinA | 17 |
| encoderPinB | 18 |
| encoderPinSW | 19 |

Notes:
- OLED mode uses encoder pins. LCD mode uses legacy button pins.
- Encoder polarity/pull mode is controlled by `encoderCommonPositive` in `src/config.h`.

## Configuration Guide

Main tuning location:
- `src/globals.cpp` (default values)
- `src/config.h` (extern declarations and shared parameters)

UI behavior location:
- `src/io_panel.cpp` (OLED + rotary encoder menu logic)

Recommended parameters to verify before real charging tests:

| Group | Parameters |
|---|---|
| Battery window | `voltageBatteryMax`, `voltageBatteryMin` |
| Charge limit | `currentCharging` |
| Absolute safety caps | `currentInAbsolute`, `currentOutAbsolute`, `temperatureMax` |
| PWM | `pwmFrequency`, `pwmResolution`, `PWM_MaxDC`, `PPWM_margin` |
| Voltage calibration | `inVoltageDivRatio`, `outVoltageDivRatio` |
| Current calibration | `currentMidPoint`, `currentSensV` |
| Thermal calibration | `ntcResistance` |

Encoder/UI parameters to verify:

| Group | Parameters |
|---|---|
| Encoder wiring mode | `encoderCommonPositive` |
| Serial telemetry mode | `serialTelemMode` |

## Telemetry Setup

Credentials currently live in `src/globals.cpp`:
- `auth`
- `ssid`
- `pass`

Behavior:
- Wi-Fi connection is attempted when `enableWiFi = 1`, even if Blynk is not available.
- If `BlynkSimpleEsp32.h` is present, cloud telemetry is enabled and auto-reconnect is used.
- If Blynk headers are not present, firmware still compiles and runs without Blynk cloud telemetry.
- Onboard serial telemetry output is gated to active serial listener conditions (board-dependent).
- On USB-UART bridge boards, monitor-open detection may be limited by hardware/driver behavior.

### Blynk Requirements

- `platformio.ini` includes `blynkkk/Blynk@^1.3.2` in `lib_deps`.
- `auth` in `src/globals.cpp` must match your Blynk template token.
- ESP32 must connect to a 2.4 GHz Wi-Fi network.

### Blynk Dashboard Datastream Mapping

Create datastreams/widgets on the following virtual pins:

| Pin | Value | Suggested Widget |
|---|---|---|
| V1 | `powerInput` | Value Display / Gauge |
| V2 | `batteryPercent` | Gauge |
| V3 | `voltageInput` | Gauge |
| V4 | `currentInput` | Gauge |
| V5 | `voltageOutput` | Gauge |
| V6 | `currentOutput` | Gauge |
| V7 | `temperature` | Value Display / Gauge |
| V8 | `Wh / 1000` (kWh) | Chart / Value Display |
| V9 | `energySavings` | Value Display |
| V10 | Buck state LED | LED |
| V11 | Battery-full LED | LED |
| V12 | Low-battery LED | LED |
| V13 | Input-OK LED | LED |
| V14 | `voltageBatteryMin` | Value Display |
| V15 | `voltageBatteryMax` | Value Display |
| V16 | `currentCharging` | Value Display |
| V17 | `electricalPrice` | Value Display |

Current firmware behavior is telemetry-out only for Blynk (no `BLYNK_WRITE` control handlers yet).

### Wi-Fi/Blynk Troubleshooting

- Verify SSID/password in `src/globals.cpp` are exact (case-sensitive).
- Confirm router broadcasts 2.4 GHz and has DHCP enabled.
- Check `enableWiFi` is set to `1`.
- Confirm Blynk token (`auth`) belongs to the same template where datastreams are created.
- If upload fails with port busy, close serial monitor before flashing.

## Validation Checklist (Recommended)

Before connecting full-power hardware:
- Validate ADC voltage scaling with a trusted multimeter
- Validate current sensor offset and slope at no-load and known load
- Verify fan trigger and overtemperature threshold behavior
- Force low-voltage and overcurrent conditions safely to confirm fault handling
- Confirm EEPROM save/load and menu setting persistence

## Safety Notice

High-current DC systems can be dangerous.

Minimum safety practices:
- Use proper fusing and DC-rated breakers
- Use correct wire gauge and secure terminations
- Provide heatsinking and airflow for power components
- Keep low-voltage testing isolated from high-energy battery banks

You are responsible for assembly, validation, and operation of your hardware.

## Roadmap

- Move sensitive credentials to safer configuration handling
- Add unit tests for charging/protection logic
- Add persistent fault history and event timestamps
- Implement Bluetooth telemetry path
- Add battery chemistry presets (for example, lead-acid and LiFePO4)

## Attribution

Original inspiration/reference:
- https://www.instructables.com/DIY-1kW-MPPT-Solar-Charge-Controller/

If you reuse this project, keep attribution to both the reference design and this improved implementation.

## Contributing

Contributions are welcome.

For pull requests, include:
- What changed
- Why it changed
- How it was tested (logs, measurements, or hardware test notes)
