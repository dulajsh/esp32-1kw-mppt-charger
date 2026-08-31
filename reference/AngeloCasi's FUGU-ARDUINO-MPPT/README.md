# Original Reference: Angelo Casi's FUGU-ARDUINO-MPPT (1kW MPPT Solar Charge Controller)

This directory contains the original reference design files, engineering calculators, schematics, PCB Gerbers, and legacy firmware created by **Angelo Casi (TechBuilder)** for the **DIY 1kW MPPT Solar Charge Controller (FUGU)**.

---

## 📌 Project Overview & Links

* **Original Creator:** Angelo Casi ([TechBuilder](https://www.instructables.com/member/Angelo+Casimiro/))
* **Instructables Guide:** [DIY 1kW MPPT Solar Charge Controller](https://www.instructables.com/DIY-1kW-MPPT-Solar-Charge-Controller/)
* **Project Reference Name:** FUGU-ARDUINO-MPPT / FUGU-ESP32 MPPT

### Key Design Specifications
| Parameter | Specification | Notes |
|---|---|---|
| **Topology** | Synchronous Buck Converter | High-side & Low-side N-Channel MOSFETs |
| **Max PV Input Voltage ($V_{in}$)** | $80\text{ V}$ ($V_{oc} < 80\text{ V}$) | Absolute hardware maximum limit |
| **Battery Voltage Range ($V_{out}$)** | $10\text{ V} - 60\text{ V}$ | Supports 12V, 24V, 36V, and 48V nominal banks |
| **Max Continuous Output Current** | $30\text{ A} - 40\text{ A}$ | Dependent on heatsink size and active fan cooling |
| **Max Power Output ($P_{max}$)** | $\approx 1000\text{ W} \text{ (1 kW)}$ | Scalable based on thermal design |
| **Efficiency** | Up to $96\% - 98\%$ | Varies with input/output differential & load |
| **MCU Platform** | ESP32 (WROOM-32 / Feather ESP32) | Dual-core 240 MHz, hardware PWM & Wi-Fi |
| **ADC Subsystem** | Adafruit ADS1015 (12-bit) / ADS1115 (16-bit) | Dedicated high-precision external I²C ADC |
| **Current Sensing** | Hall-Effect Current Sensor (e.g. ACS712 / ACS758 / ACS770) | Isolated high-current measurement |
| **Display / User Interface** | 16×2 I²C LCD or 0.96" SSD1306 OLED | Rotary encoder or 4-button navigation |
| **Thermal Protection** | 100k NTC thermistor | Active 12V fan speed regulation & thermal shutdown |

---

## 📂 Directory Contents & File Descriptions

```text
reference/AngeloCasi's FUGU-ARDUINO-MPPT/
├── README.md                          # This reference overview documentation
├── ARDUINO_MPPT_FIRMWARE_V1.1.1.zip   # Original Arduino sketch firmware bundle (v1.1.1)
├── Parts List.xlsx                    # Complete Bill of Materials (BOM) & component specs
├── Inductor Calculator.xlsx           # Custom toroidal inductor design and winding tool
├── TechBuilder - MPPT CALCULATOR.xlsx # PV array sizing, duty cycle, and battery parameter tool
├── Efficiency Curve Test.xlsx         # Bench test logs and efficiency measurements
├── main-board-schematic.png           # High-resolution full schematic diagram
├── main-board-proteus.pdsprj          # Proteus ISIS / ARES project file (schematics & layout)
└── main-board-gerber.zip              # Manufacturing Gerber package for PCB fabrication
```

### 1. Hardware & Manufacturing Files
* **`main-board-schematic.png`**: High-resolution circuit schematic showing the power stage, gate drivers (IR2104 / discrete drivers), buck inductor, buck capacitors, ESP32 connections, ADS1015 ADC wiring, and sensor front-ends.
* **`main-board-proteus.pdsprj`**: Proteus CAD project file containing the full schematic and double-sided PCB layout.
* **`main-board-gerber.zip`**: Production-ready RS-274X Gerber and Excellon drill files for ordering bare PCBs from fabrication houses (e.g., JLCPCB, PCBWay).
* **`Parts List.xlsx`**: Detailed spreadsheet listing all passives, active semiconductors (MOSFETs, gate drivers, regulators), connectors, sensors, and mechanical hardware.

### 2. Engineering Calculators & Test Sheets
* **`Inductor Calculator.xlsx`**: Tool for calculating custom toroidal inductor parameters:
  * Core selection (Kool Mμ / Sendust / Iron Powder - e.g., T157-2, MS-157125-2)
  * Required inductance ($L$), number of turns ($N$), wire gauge (AWG), and parallel strands
  * Current ripple ($\Delta I_L$) and core saturation margin at peak currents
* **`TechBuilder - MPPT CALCULATOR.xlsx`**: System dimensioning calculator for solar panel string configuration, open-circuit voltage ($V_{oc}$), maximum power point voltage ($V_{mp}$), battery charging stages, and theoretical duty cycle range.
* **`Efficiency Curve Test.xlsx`**: Empirical bench-test data logging input power, output power, heat dissipation, and efficiency curves across various voltage levels and load currents.

### 3. Firmware Archive
* **`ARDUINO_MPPT_FIRMWARE_V1.1.1.zip`**: The original Arduino IDE codebase (v1.1.1) containing:
  * Perturb and Observe (P&O) MPPT tracking loop
  * Lead-acid / LiFePO4 multi-stage charging (Bulk, Absorption, Float)
  * 4-button menu and 16×2 LCD user interface
  * Legacy Blynk integration

---

## ⚡ Relationship to this Repository

The root project in this repository is a **heavily refactored and modernized rewrite** of Angelo Casi's original concept, adapted for PlatformIO with:
1. **Modular Code Architecture**: Header files placed in [`include/`](../../include/) and source files in [`src/`](../../src/).
2. **Dual Core FreeRTOS Execution**: Core 1 handles fast control/MPPT/protection loops, Core 0 manages Wi-Fi, OTA, and cloud telemetry.
3. **Advanced UI**: Support for high-resolution SSD1306 128×64 OLED with rotary encoder, alongside legacy 16×2 LCD support.
4. **Enhanced Safety Protection**: Predictive PWM startup, input/output over-current and over-voltage trips, backflow prevention, and thermal derating.