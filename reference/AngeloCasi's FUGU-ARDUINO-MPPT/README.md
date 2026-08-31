# Original Reference: Angelo Casi's FUGU-ARDUINO-MPPT (1kW MPPT Solar Charge Controller)

This directory contains the complete archive of original reference design files, engineering calculators, schematics, PCB Gerbers, and legacy firmware created by **Angelo Casi (TechBuilder)** for the **DIY 1kW MPPT Solar Charge Controller (FUGU)**.

---

## 📌 Project Overview & Links

* **Original Creator:** Angelo Casi ([TechBuilder](https://www.instructables.com/member/Angelo+Casimiro/))
* **Instructables Guide:** [DIY 1kW MPPT Solar Charge Controller (100% Free)](https://www.instructables.com/DIY-1kW-MPPT-Solar-Charge-Controller/)
* **Project Reference Name:** FUGU-ARDUINO-MPPT / FUGU-ESP32 MPPT
* **Hardware Generation:** V1.1 / V1.2 Main Board

---

## ⚡ Technical Specifications

| Parameter | Specification | Engineering Details & Limits |
|---|---|---|
| **Topology** | Synchronous Buck Converter | High-side & Low-side N-Channel MOSFET switching |
| **Max PV Input Voltage ($V_{in}$)** | $80\text{ V}$ ($V_{oc} < 80\text{ V}$) | Limited by input capacitor voltage & MOSFET $V_{ds}$ ratings |
| **Min PV Input Voltage ($V_{in}$)** | $15\text{ V}$ | Must be higher than battery voltage ($V_{in} > V_{bat} + \text{dropout}$) |
| **Battery Compatibility ($V_{out}$)** | $10\text{ V} - 60\text{ V}$ | 12V, 24V, 36V, and 48V nominal battery configurations |
| **Max Continuous Output Current** | $30\text{ A} - 40\text{ A}$ | With active fan cooling and low $R_{ds(on)}$ MOSFETs |
| **Max Power Handling ($P_{max}$)** | $\approx 1000\text{ W} \text{ (1 kW)}$ | e.g. $\sim 30\text{ A} \times 33.6\text{ V}$ (24V system) or $\sim 20\text{ A} \times 50\text{ V}$ (48V system) |
| **Peak Efficiency** | $96\% - 98.2\%$ | Lowest losses when $V_{in}$ is close to $V_{mp}$ of the battery |
| **Switching Frequency ($f_{sw}$)** | $40\text{ kHz} - 50\text{ kHz}$ | Optimal trade-off between switching losses & inductor size |
| **Microcontroller Platform** | ESP32-WROOM-32 (NodeMCU / Feather) | Dual 32-bit Xtensa LX6 cores @ 240 MHz, hardware LEDC PWM |
| **ADC Subsystem** | Adafruit ADS1015 (12-bit) / ADS1115 (16-bit) | External high-precision I²C ADC with programmable gain |
| **Current Sensing Method** | Hall-Effect Sensor IC (ACS712 / ACS758 / ACS770) | Galvanically isolated, bi-directional current measurement |
| **Gate Driver IC** | IR2104 / IR2104S (Half-Bridge Driver) | High-voltage bootstrap driver with internal dead-time (~520ns) |
| **User Interface** | 16×2 I²C Character LCD / 0.96" SSD1306 OLED | 4 Push Buttons (Select, Back, Left, Right) or Rotary Encoder |
| **Thermal Management** | Active PWM / On-Off Fan + 100k NTC Thermistor | Heatsink temperature monitoring with auto-shutdown ($>60^\circ\text{C}$) |

---

## 📂 File Inventory & Technical Description

```text
reference/AngeloCasi's FUGU-ARDUINO-MPPT/
├── README.md                          # Comprehensive technical reference manual
├── ARDUINO_MPPT_FIRMWARE_V1.1.1.zip   # Legacy Arduino IDE firmware archive (v1.1.1)
├── Parts List.xlsx                    # Detailed Bill of Materials (BOM) with component ratings
├── Inductor Calculator.xlsx           # Toroidal inductor design, core selection & winding tool
├── TechBuilder - MPPT CALCULATOR.xlsx # Solar array sizing, MPPT duty cycle & power estimator
├── Efficiency Curve Test.xlsx         # Empirical bench measurements & efficiency logs
├── main-board-schematic.png           # Complete circuit schematic diagram
├── main-board-proteus.pdsprj          # Proteus ISIS schematic and ARES PCB layout source
└── main-board-gerber.zip              # Manufacturing Gerber files (RS-274X) for PCB fabrication
```

### 1. Circuit Design & Manufacturing Files
* **`main-board-schematic.png`**:
  * **Power Stage:** IRFB4110 / IRF3205 / N-Channel MOSFETs arranged in half-bridge buck configuration.
  * **Gate Drive:** IR2104 half-bridge driver powered by a 12V auxiliary rail, generating high-side bootstrap drive ($V_B - V_S$) and complementary low-side switching.
  * **Backflow Prevention:** Output P-channel or N-channel MOSFET switch preventing reverse current from battery to solar panel during night/low-light.
  * **Auxiliary Power:** Multi-stage buck/linear regulators stepping down high PV voltage to +12V (gate drive / fan) and +5V / +3.3V (ESP32 and logic).
* **`main-board-proteus.pdsprj`**: Complete Labcenter Proteus 8 CAD design package. Can be used for circuit simulation, schematic modifications, and PCB layout editing.
* **`main-board-gerber.zip`**: Standard RS-274X layer stack and NC drill files formatted for fabrication with JLCPCB, PCBWay, OSHPark, etc.
* **`Parts List.xlsx`**: Detailed BOM itemizing:
  * Power MOSFETs ($V_{ds} \ge 100\text{V}$, ultra-low $R_{ds(on)} < 5\text{ m}\Omega$)
  * High-frequency, low-ESR electrolytic and ceramic filter capacitors ($100\text{V}$ rated)
  * Current sensor ICs, ADS1015/ADS1115 ADC breakout modules
  * Shottky freewheeling diodes, TVS surge protection diodes, and 12V cooling fans

### 2. Design Calculators & Test Data
* **`Inductor Calculator.xlsx`**:
  * Calculates core geometry and required magnetic properties (Sendust / Kool Mμ / High Flux / Iron Powder).
  * Computes required inductance:
    $$L = \frac{V_{out} \times (V_{in} - V_{out})}{\Delta I_L \times f_{sw} \times V_{in}}$$
  * Determines number of turns ($N$), wire diameter, and parallel copper strands (bifilar/trifilar winding) to minimize skin effect and $I^2R$ copper losses at $40\text{--}50\text{ kHz}$.
* **`TechBuilder - MPPT CALCULATOR.xlsx`**:
  * Computes panel series/parallel string arrangements ($V_{mp}$, $I_{mp}$, $V_{oc}$, $I_{sc}$).
  * Computes theoretical duty cycle ($D = V_{out} / V_{in}$) and predicts buck output parameters across solar irradiance levels.
* **`Efficiency Curve Test.xlsx`**:
  * Contains real benchtop test logs capturing efficiency curves across varying loads ($5\text{A}$ to $35\text{A}$) and voltage deltas.

### 3. Firmware Archive
* **`ARDUINO_MPPT_FIRMWARE_V1.1.1.zip`**:
  * Original Arduino `.ino` monolithic firmware implementing:
    * Perturb and Observe (P&O) MPPT tracking algorithm
    * Constant Current (CC) & Constant Voltage (CV) charging phases
    * Basic LCD display routines and 4-button menu state machine
    * Legacy Blynk IoT integration for remote data monitoring

---

## 🔄 Upgrades in the Main Repository

The modernized firmware in the root of this repository ([`esp32-1kw-mppt-charger`](../../)) builds on Angelo Casi's original hardware concept with the following software and architectural enhancements:

1. **PlatformIO & Modern C++ Structure**:
   * Header files organized under [`include/`](../../include/) and implementation under [`src/`](../../src/).
   * Clean separation of concerns: `charging`, `protection`, `sensors`, `system`, `telemetry`, `io_panel`, and `lcd`.
2. **Dual-Core FreeRTOS Partitioning**:
   * **Core 1:** Time-critical high-frequency sensor sampling, MPPT tracking, PWM modulation, and fast protection loops.
   * **Core 0:** Asynchronous background tasks including Wi-Fi, Blynk IoT, and Arduino OTA.
3. **Advanced OLED & Rotary Encoder UI**:
   * High-contrast SSD1306 OLED interface with dynamic graph displays and intuitive rotary encoder navigation.
   * Backward compatibility with legacy 16×2 I²C LCD screens.
4. **Enhanced Hardware Protection Logic**:
   * Predictive PWM startup (prevents sudden high inrush currents upon converter engagement).
   * Multi-stage safety checks: Input Overcurrent (IOC), Input Undervoltage (IUV), Output Overvoltage (OOV), Output Overcurrent (OOC), and Overtemperature (OTE).
   * Safe calibration mode that disables PWM while altering charging settings.