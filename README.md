# ESP32 1kW MPPT Solar Charge Controller (Improved)

Firmware for a high-power DIY solar charge controller using ESP32, ADS1015, and a buck stage with MPPT/CV behavior.

Reference project (inspiration):
- Original Guide: [Instructables - DIY 1kW MPPT Solar Charge Controller](https://www.instructables.com/DIY-1kW-MPPT-Solar-Charge-Controller/)
- Reference Files & Schematics: [`reference/AngeloCasi's FUGU-ARDUINO-MPPT/README.md`](reference/AngeloCasi%27s%20FUGU-ARDUINO-MPPT/README.md)

This fork focuses on cleaner structure, practical protection logic, easier tuning, and smoother PlatformIO development.

## At A Glance

| Area | Details |
|---|---|
| MCU | ESP32 (Dual-core 240MHz, PlatformIO target: `featheresp32` / `featheresp32_ota`) |
| Control | MPPT Solar Charger (P&O / Non-MPPT) + Dedicated Lab Bench PSU Mode (CV/CC) |
| Performance | Dedicated Core 1 power conversion loop running at ~120–130 Hz |
| Sensing | ADS1115 / ADS1015 external ADC at 860 SPS + NTC internal temperature sensor |
| UI | Auto-detected UI: 0.96in I2C OLED (SSD1306) + rotary encoder or 16x2 I2C LCD + buttons |
| Telemetry | High-speed serial telemetry (500k baud) + Blynk IoT Cloud telemetry |
| Firmware OTA | Wireless Over-The-Air flashing via ArduinoOTA with hardware safety interlock |
| Battery Presets | Lead-Acid, AGM, GEL, LiFePO4, Li-Ion, and Custom (Bulk, Absorption, Float) |
| Persistence | EEPROM storage for all calibration, charger, and PSU parameters |
| Safety | Multi-tier hardware protection: IUV, IOC, OOV, OOC, OTE, FLV, BNC, backflow isolation |

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
# Option A: Upload via USB Serial
pio run -t upload -e featheresp32

# Option B: Upload wirelessly via OTA (Over-The-Air)
pio run -t upload -e featheresp32_ota
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

### 1. Dual-Core Real-Time Architecture
- **Core 1 Dedicated Power Engine (`loop()`):** High-speed deterministic execution (~120–130 Hz) handling ADS1115 ADC sampling, hardware protection, and the buck control loop. Free from display, serial, and networking delays.
- **Core 0 High-Priority UI Task (`uiTask`, Priority 2):** Runs at 200 Hz for instant rotary encoder detent sampling and button response, with display rendering throttled to 25 FPS (40ms) to prevent I2C bus congestion.
- **Core 0 Background Task (`coreTwo`, Priority 1):** Handles background WiFi networking, Blynk cloud sync, ArduinoOTA service, fan thermal management, and serial telemetry.
- **Thread-Safe I2C Mutex (`i2cMutex`):** Hardware I2C bus arbitration protecting ADS1115 conversions on Core 1 from OLED / LCD transmissions on Core 0.

### 2. Dual Operating Modes (Charger vs. Lab PSU)
- **Solar Charger Mode (`output_Mode = 1`):**
  - Advanced MPPT tracking (Perturb & Observe) or direct CC/CV regulated charging.
  - Multi-stage battery charging: **Bulk**, **Absorption**, and **Float** with automatic absorption current timeout.
  - Built-in battery chemistry presets: **Lead-Acid**, **AGM**, **GEL**, **LiFePO4**, **Li-Ion**, and **Custom**.
- **Lab Power Supply Mode (`output_Mode = 0`):**
  - Operates as a constant-voltage (CV) / constant-current (CC) regulated DC bench supply.
  - Dedicated target parameters: `psuVoltageTarget` (1.2V–60.0V) and `psuCurrentLimit` (0.1A–40.0A) with EEPROM persistence.
  - Multi-tier dynamic error stepping for rapid transient recovery under dynamic, variable loads (e.g. high-power audio amplifiers).
  - Tight 30mV stability deadband to eliminate steady-state voltage ripple and PWM hunt.

### 3. Protection and Reliability
- Input undervoltage detection (IUV) with auto-bypass in PSU mode
- Input overcurrent detection (IOC) and output overcurrent detection (OOC)
- Output overvoltage detection (OOV) with instant buck shutdown
- Overtemperature detection (OTE) with active fan cooling and thermal derating
- Flat voltage (FLV) and battery-not-connected (BNC) safety interlocks
- Low-loss backflow ideal MOSFET control for reverse current isolation

### 4. Over-The-Air (OTA) Wireless Updates
- Wireless flashing over local network (`featheresp32_ota` environment via port 3232).
- **Hardware Safety Interlock:** Instantly drives buck PWM to 0, disables buck driver, opens backflow MOSFET, and turns off fan upon OTA start.
- **Dedicated OLED OTA Screen:** Displays real-time flash status, centered 100-pixel progress bar, percentage, and reboot countdown.
- Safe execution: UI task handles display updates at max 10 FPS only on state/percent changes to avoid starving flash write operations.

### 5. User Interface & Rotary Encoder Control
- **Auto UI Selection:** Auto-detects SSD1306 OLED (128x64) + rotary encoder (default) or 16x2 I2C LCD + navigation buttons.
- **Calibrated Rotary Encoder Curve:** Lab-instrument feel:
  - Slow/individual clicks: exact $1\times$ step (0.01V / 0.01A per detent).
  - Brisk turns: smooth dynamic acceleration up to $50\times$ for rapid voltage spans.
  - 400ms idle reset guarantees precision when landing on target values.
- **6-Page OLED Dev View:**
  1. Status, operating mode, and efficiency
  2. Power, Wh/kWh/MWh accumulated energy, and runtime
  3. PWM duty, SOC battery percentage, and source status
  4. Real-time loop performance (`loopTime` in ms) and control metrics
  5. Raw ADS1115 ADC voltages (A0, A1, A2, A3)
  6. Comprehensive error & protection diagnostics (IUV, OOV, BNC, IOC, OOC, OTE, FLV, ADC health, fault counter)
- **Safe Editing Mode:** Converter switching automatically pauses when entering the OLED settings table.

### 6. Embedded Single-Page Local Web UI
- **Zero-Cloud Local Dashboard:** Fully self-contained dark-mode single-page dashboard served directly by the ESP32 on port 80. Zero Blynk or cloud dependencies.
- **Instant Browser Access:** Open `http://fugu-mppt-1kw.local/` (via mDNS) or the device IP address on any phone, tablet, or desktop on the local Wi-Fi.
- **Real-Time Live Telemetry (1s Refresh):** Inline dynamic fetch (`/?data=1`) updates all cards smoothly without page refreshes or screen flickering:
  - **Header & Status:** Active operation mode badge (`PSU MODE: CV/CC` vs `CHARGER: BULK/ABS/FLOAT`), system uptime, and connection info.
  - **Solar / DC Input:** Input Voltage ($V_{in}$), Current ($I_{in}$), Power ($P_{in}$), and Source OK status.
  - **Output / Battery / Load:** Output Voltage ($V_{out}$), Current ($I_{out}$), Output Power ($P_{out}$), and Target Setpoints ($V_{target}$, $I_{limit}$).
  - **Power Converter & Thermals:** Conversion Efficiency (%), Heatsink Temperature (°C), Cooling Fan status, PWM Duty Cycle (raw & %), and Core 1 loop time / frequency.
  - **Battery & Energy Tracking:** Battery SOC percentage bar, accumulated energy ($Wh$ & $kWh$), money savings ($), and battery preset name.
  - **Hardware Diagnostics:** Live color-coded status badges for all protection trips (IUV, OOV, IOC, OOC, OTE, BNC, FLV, ADS1115 ADC health).
- **Core 1 Isolation:** Web server runs on Core 0 within `coreTwo` and automatically pauses during OTA updates, completely eliminating interference with the high-speed ~130 Hz buck converter loop.

## Firmware Architecture

```
                       +-----------------------------------+
                       |          ESP32 Dual-Core          |
                       +-----------------+-----------------+
                                         |
               CORE 1                    |                 CORE 0
       (Pure Power Engine)               |          (UI & Connectivity)
+---------------------------------+      | +-------------------------------------+
| loop() (~120-130 Hz):           |      | | uiTask (Priority 2, 200 Hz):        |
|  - Read_Sensors()               |      | |  - updateEncoder() / updateButton() |
|    * Fast ADS1115 (860 SPS)     |      | |  - IO_Panel_Update()                |
|  - Device_Protection()          |      | |  - 25 FPS display limiter (40ms)    |
|  - Charging_Algorithm()         |      | +-------------------------------------+
|    * Multi-tier CV/CC loop      |      |
|    * MPPT P&O / Multi-stage     |      | +-------------------------------------+
+---------------------------------+      | | coreTwo (Priority 1, Background):   |
                 |                       | |  - Wireless_Telemetry() (Blynk/WiFi)|
                 |                       | |  - ArduinoOTA Handler               |
                 v                       | |  - System_Processes() (Fan control) |
      +---------------------+            | |  - Onboard_Telemetry() (Serial)     |
      |   i2cMutex Guard    |<-----------+ +-------------------------------------+
      +---------------------+
                 |
                 v
   [ Shared Hardware I2C Bus: SDA=21, SCL=22 ]
         |                       |
         v                       v
  ADS1115 (0x48)          SSD1306 OLED (0x3C)
```

## Project Layout

```text
.
|-- platformio.ini
|-- include/
|   |-- config.h
|   |-- charging.h
|   |-- protection.h
|   |-- sensors.h
|   |-- telemetry.h
|   |-- web_page.h
|   |-- lcd.h
|   |-- io_panel.h
|   `-- system.h
|-- src/
|   |-- main.cpp
|   |-- globals.cpp
|   |-- charging.cpp
|   |-- protection.cpp
|   |-- sensors.cpp
|   |-- telemetry.cpp
|   |-- lcd.cpp
|   |-- io_panel.cpp
|   `-- system.cpp
|-- reference/
|   `-- AngeloCasi's FUGU-ARDUINO-MPPT/
|       |-- README.md
|       |-- main-board-schematic.png
|       |-- main-board-proteus.pdsprj
|       |-- main-board-gerber.zip
|       |-- Parts List.xlsx
|       |-- Inductor Calculator.xlsx
|       |-- TechBuilder - MPPT CALCULATOR.xlsx
|       |-- Efficiency Curve Test.xlsx
|       `-- ARDUINO_MPPT_FIRMWARE_V1.1.1.zip
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
- Encoder polarity/pull mode is controlled by `encoderCommonPositive` in `include/config.h`.

## Configuration Guide

Main tuning location:
- `src/globals.cpp` (default values)
- `include/config.h` (extern declarations and shared parameters)

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

## Roadmap & Progress

- [x] **Dual-Core Architecture:** Dedicated Core 1 power engine (~130 Hz) + Core 0 UI & network tasks
- [x] **Thread-Safe I2C Arbitration:** FreeRTOS mutex preventing ADC and OLED/LCD bus collisions
- [x] **High-Speed ADC Sampling:** ADS1115 configured for 860 SPS with single-ended fast reads
- [x] **Dedicated Lab Bench PSU Mode:** Constant-Voltage (CV) and Constant-Current (CC) regulation with independent parameters
- [x] **Dynamic Load Optimization:** Multi-tier proportional stepping for variable dynamic loads (e.g., audio amplifiers)
- [x] **Multi-Stage Battery Presets:** Lead-Acid, AGM, GEL, LiFePO4, Li-Ion, and Custom (Bulk, Absorption, Float)
- [x] **Wireless OTA Firmware Flashing:** ArduinoOTA service with hardware shutdown safety interlock and live OLED progress
- [x] **Rotary Encoder Precision Tuning:** Calibrated lab-grade acceleration curve (0.01V fine steps) and 25 FPS display limiter
- [x] **Diagnostic Dev View:** 6 OLED pages including real-time error status, raw ADC channels, and loop cycle timing
- [x] **Embedded Single-Page Web UI:** Zero-cloud responsive dark-mode browser dashboard (1s live telemetry) on port 80 / mDNS
- [ ] Move sensitive credentials to safer configuration handling (e.g., WiFi manager / secrets header)
- [ ] Add persistent fault history and event logs in EEPROM
- [ ] Implement Bluetooth Low Energy (BLE) telemetry path
- [ ] Add unit tests for power conversion and protection logic

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
