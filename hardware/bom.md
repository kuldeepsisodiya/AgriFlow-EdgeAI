# Bill of Materials (BOM) & Component Engineering Analysis

This document outlines the complete bill of materials for **AgriFlow-EdgeAI**, detailing component electrical specifications, unit costs, and engineering justifications for hackathon prototypes and agricultural deployments.

---

## Component Summary Matrix

| Ref | Component | Model / Part No. | Key Electrical Specifications | Qty | Est. Cost (INR) | Est. Cost (USD) |
| :--- | :--- | :--- | :--- | :---: | :---: | :---: |
| **U1** | Microcontroller | ESP32 DevKit V1 (ESP-WROOM-32) | Dual-core 240MHz, 520KB SRAM, 4MB Flash, Wi-Fi 802.11 b/g/n | 1 | ₹380.00 | $4.60 |
| **S1** | Soil Moisture Sensor | Capacitive v1.2 | Operating: 3.3V–5V, Analog Output: 1.2V–3.0V, Frequency Modulated | 1 | ₹95.00 | $1.15 |
| **S2** | Reservoir Level Probe | Immersion Conductive Strip | Operating: 3.3V–5V, Output: 0.0V–2.8V, Depth: 0–40mm | 1 | ₹60.00 | $0.72 |
| **S3** | Temp & Humidity | DHT22 (AM2302) | Temp: -40 to 80°C (±0.5°C), RH: 0–100% (±2%), Single-bus | 1 | ₹240.00 | $2.90 |
| **K1** | Relay Actuator | 1-Channel 5V Relay Module | PC817 Optoisolator, 10A 250VAC / 10A 30VDC, Active LOW trigger | 1 | ₹75.00 | $0.90 |
| **M1** | Water Pump | 12V DC R385 / Submersible | 12V DC, 0.8A–1.2A, Flow: 1.5–2.0 L/min, Head Lift: 1.5m | 1 | ₹160.00 | $1.95 |
| **VR1**| DC-DC Buck Converter| LM2596 Step-Down Module | Input: 4.5V–40V, Output tuned to 5.05V DC, 3A Max, 92% efficiency | 1 | ₹85.00 | $1.05 |
| **PS1**| Power Supply | 12V 2A DC SMPS Adapter | Input: 100–240VAC, Output: 12V DC 2.0A (24W), 5.5×2.1mm barrel | 1 | ₹250.00 | $3.00 |
| **D1** | Flyback Diode | 1N4007 | 1000V 1A Silicon Rectifier (Reverse bias across pump leads) | 1 | ₹2.00 | $0.03 |
| **C1** | Filter Capacitors | Electrolytic + Ceramic Set | 100µF 25V (Bulk rail decoupling) + 100nF 50V (High-freq filter) | 2 | ₹10.00 | $0.12 |
| **R1** | Pull-up & Limiting | Resistor Assortment | 10kΩ 1/4W (DHT22 Pull-up) + 330Ω 1/4W (Status LED) | 2 | ₹2.00 | $0.03 |
| **LED**| Telemetry Annunciator| 5mm RGB / WS2812B | Common Cathode, 20mA max forward current | 1 | ₹15.00 | $0.18 |
| **TB1**| Irrigation Tubing | Food-Grade Silicone | 8mm OD, 6mm ID, High flexibility, UV resistant | 2m | ₹80.00 | $0.96 |
| **ENC**| Field Enclosure | IP65 Weatherproof Junction Box| 158×90×60mm ABS Plastic with Rubber Gaskets & Cable Glands | 1 | ₹350.00 | $4.25 |
| **—** | **TOTAL** | — | **Complete Turnkey Field Deployment Unit** | **—** | **₹1,789.00** | **~$21.84** |

---

## Component Engineering Justifications

### 1. Capacitive Soil Moisture v1.2 vs Resistive Probes
- **Problem with Resistive Probes**: Standard resistive probes (two exposed copper traces) suffer from rapid electrolysis and galvanic corrosion when direct DC current is passed through wet soil, typically failing within 2–4 weeks.
- **Why Capacitive v1.2**: Utilizes an onboard TLC555 timer generating a high-frequency alternating signal. The probe measures soil dielectric permittivity without exposing metal to soil moisture, delivering multi-year sensor lifespan.

### 2. ADC1 vs ADC2 on the ESP32
- **The Wi-Fi Conflict**: The ESP32 integrates two Successive Approximation Register (SAR) ADCs. However, ADC2 is shared with the Wi-Fi baseband driver. Calling `analogRead()` on ADC2 pins (e.g., GPIO 0, 2, 4, 12–15, 25–27) while Wi-Fi is transmitting results in corrupted or failed conversions.
- **Our Selection**: `GPIO35` (ADC1_CH7) for soil moisture and `GPIO34` (ADC1_CH6) for reservoir level completely bypass ADC2, allowing concurrent TLS Wi-Fi streaming and continuous analog sampling.

### 3. Optocoupled Relay Isolation & Inductive Flyback Protection
- **Optical Isolation**: The PC817 optocoupler galvanically isolates the ESP32 3.3V logic from the 12V inductive motor loop.
- **Flyback Diode (1N4007)**: When a DC motor or relay coil is suddenly de-energized, the collapsing magnetic field produces a reverse inductive spike of up to several hundred volts ($V = -L \cdot \frac{di}{dt}$). Placing a 1N4007 diode in reverse-bias directly across the pump terminals clamps this spike safely, protecting the relay contacts from arcing and preventing MCU resets.
