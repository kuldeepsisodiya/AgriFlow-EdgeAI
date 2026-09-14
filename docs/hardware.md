# Hardware Specifications & Transducer Interfacing

This document provides in-depth technical analysis, electrical datasheets, and circuit interfacing principles for the components comprising **AgriFlow-EdgeAI**.

---

## 1. Microcontroller: ESP32 DevKit V1 (ESP-WROOM-32)

```text
       ┌───────────────────────────────┐
       │     ESP32-WROOM-32 Module     │
       │   Dual-Core Xtensa LX6 @240MHz │
       │      520 KB SRAM / 4MB Flash  │
       │   802.11 b/g/n Wi-Fi + BLE    │
       └───────────────┬───────────────┘
                       │
       ┌───────────────┴───────────────┐
       │      SAR ADC1 Subsystem       │
       │  GPIO35 (Soil) / GPIO34 (Tank)│
       │   12-bit Resolution (0..4095) │
       │  *Immune to Wi-Fi RF activity │
       └───────────────────────────────┘
```

- **Processor**: Dual-core 32-bit Xtensa LX6 delivering up to 600 DMIPS at 240 MHz.
- **Analog Subsystems**:
  - Contains two 12-bit Successive Approximation Register (SAR) ADCs.
  - **ADC1** is assigned to `GPIO35` (Soil Moisture) and `GPIO34` (Reservoir Level). ADC1 operates completely independently from the Wi-Fi RF baseband, ensuring uninterrupted analog sampling.
- **Power Consumption**:
  - Active RF Transmission: ~160–240 mA
  - CPU Processing / Idle: ~30–50 mA
  - Deep Sleep Capability: ~10 µA (for future battery/solar node deployments)

---

## 2. Capacitive Soil Moisture Sensor v1.2

```text
[555 Timer IC] ──> [High-Freq Square Wave] ──> [Soil Capacitor Probe] ──> [Peak Detector & Low-Pass] ──> Analog Vout
```

### Working Principle
The Capacitive Soil Moisture Sensor v1.2 utilizes an onboard **TLC555 CMOS timer IC** configured in astable multivibrator mode, generating a continuous high-frequency oscillating square wave (~1.5 MHz). The two coplanar PCB copper traces act as the plates of an open capacitor, with the surrounding soil serving as the dielectric medium.

The dielectric constant ($\kappa$) of dry soil minerals is typically between **3 and 5**, whereas pure water possesses an exceptionally high dielectric constant of **~80**. As moisture content increases:
1. The effective capacitance ($C$) of the probe increases:
   $$C = \kappa \cdot \varepsilon_0 \cdot \frac{A}{d}$$
2. The impedance ($X_c = \frac{1}{2\pi f C}$) drops, shifting the output analog voltage downward.
3. The peak detector rectifies this into a smooth analog voltage ranging from **~3.0V in completely dry air** to **~1.2V in saturated water**.

### Why Capacitive Sensing Prevents Sensor Failure
Unlike resistive soil moisture sensors where exposed copper conducts direct DC current through the soil (causing galvanic dissociation, copper plating, and oxidation within weeks), the capacitive sensor traces are insulated beneath a solder-mask conformal coating. No DC current flows into the soil, eliminating electrolysis.

---

## 3. Water Reservoir Immersion Level Sensor

- **Operating Voltage**: 3.3V – 5.0V DC
- **Operating Current**: $< 20\text{ mA}$
- **Sensor Type**: Variable conductive trace resistance array (parallel interleaved PCB tracks).
- **Functionality**:
  - Measures liquid immersion depth (0 to 40 mm).
  - As water level rises, an increasing area of conductive traces is bridged by the liquid, lowering the pull-down resistance and proportionally raising the analog voltage output.
  - Connected to `GPIO34` (ADC1_CH6). Calibrated to trigger an anti-cavitation hard lock if water level drops below **20%**.

---

## 4. DHT22 (AM2302) Microclimate Sensor

- **Measurement Range**:
  - Temperature: -40.0°C to +80.0°C (Accuracy: $\pm 0.5^\circ\text{C}$, Resolution: 0.1°C)
  - Relative Humidity: 0.0% to 100.0% RH (Accuracy: $\pm 2.0\%\text{ RH}$, Resolution: 0.1% RH)
- **Protocol**: Single-bus bi-directional digital protocol (40-bit frame):
  - 16 bits: Relative Humidity data
  - 16 bits: Temperature data
  - 8 bits: Parity / Checksum byte
- **Interfacing**: Connected to `GPIO4` with an external 10kΩ pull-up resistor to the 3.3V rail.

---

## 5. 5V Optocoupled Relay Module & Actuation Circuit

- **Relay Model**: Songle SRD-05VDC-SL-C
  - Contact Ratings: 10A 250VAC / 10A 30VDC
  - Coil Voltage: 5.0V DC (Nominal coil current: ~70 mA)
- **Optoisolator**: Lite-On / Everlight **PC817** Phototransistor.
- **Trigger Logic**: Active LOW (Driving `GPIO5` LOW forward-biases the internal infrared LED, saturating the phototransistor and pulling the relay coil driver transistor gate HIGH).
- **Protection Network**:
  - Onboard surface-mount flyback diode across the relay coil.
  - External **1N4007** power rectifier diode soldered directly across the 12V DC pump terminals in reverse-bias configuration to absorb inductive motor kickback.

---

## 6. Power Supply Regulation: LM2596 DC-DC Buck Converter

- **Topology**: Non-isolated step-down switching regulator.
- **Input Voltage**: 12.0V DC (from 24W SMPS wall adapter).
- **Regulated Output**: **5.05V DC** (tuned via multi-turn cermet potentiometer).
- **Switching Frequency**: Fixed 150 kHz internal oscillator.
- **Thermal Performance**: Operates at 92% conversion efficiency, dissipating negligible heat at typical 350mA total load without needing a heatsink.
