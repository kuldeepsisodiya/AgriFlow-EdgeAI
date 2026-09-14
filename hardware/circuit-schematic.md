# Circuit Schematic & Interconnection Specification

This document defines the electrical wiring schematic, signal netlists, and power rail isolation practices for **AgriFlow-EdgeAI**.

---

## 1. Power Distribution Tree

```text
[12V 2A DC Adapter]
       │
       ├───> [12V DC Rail] ───> [Relay COM Contact] ───> [Relay NO Contact] ───> [12V Pump (+)]
       │                                                                            │
       │                                                                        [1N4007 Diode] (Reverse Bias)
       │                                                                            │
       │                                                                            v
       │                                                                       [GND Bus]
       │
       └───> [LM2596 Buck Converter Input]
                   │
                   └───> [5.05V DC Rail]
                               │
                               ├───> ESP32 DevKit [VIN Pin] (Onboard AMS1117 LDO -> 3.3V)
                               ├───> Relay Module [VCC Pin] (JD-VCC / VCC jumper configured)
                               └───> [100µF 25V Electrolytic + 100nF Ceramic] Decoupling Array
```

---

## 2. Complete Pin-to-Pin Wiring Netlist

### A. Power Rails
| Source | Source Pin | Destination | Destination Pin | Wire Gauge / Color |
| :--- | :--- | :--- | :--- | :--- |
| 12V Adapter | 12V (+) | LM2596 Buck | IN+ | 20 AWG (Red) |
| 12V Adapter | 12V (-) | LM2596 Buck | IN- (GND) | 20 AWG (Black) |
| 12V Adapter | 12V (+) | Relay Module | COM Terminal | 20 AWG (Red) |
| LM2596 Buck | OUT+ (5V) | ESP32 DevKit | VIN | 22 AWG (Red) |
| LM2596 Buck | OUT- (GND) | ESP32 DevKit | GND | 22 AWG (Black) |
| LM2596 Buck | OUT+ (5V) | Relay Module | VCC | 22 AWG (Red) |
| ESP32 DevKit | 3V3 Rail | Capacitive Sensor | VCC | 24 AWG (Red) |
| ESP32 DevKit | 3V3 Rail | Reservoir Level | VCC | 24 AWG (Red) |
| ESP32 DevKit | 3V3 Rail | DHT22 Sensor | Pin 1 (VCC) | 24 AWG (Red) |
| Common GND | GND Bus | All Sensors | GND | 24 AWG (Black) |

### B. Signal Netlist
| Component | Signal Pin | ESP32 Pin | GPIO | Function / Electrical Properties |
| :--- | :--- | :--- | :--- | :--- |
| **Capacitive Soil Probe** | AOUT | D35 | **GPIO35** | Analog input (0–3.0V), ADC1_CH7, 12-bit conversion |
| **Reservoir Level Probe** | AOUT | D34 | **GPIO34** | Analog input (0–2.8V), ADC1_CH6, 12-bit conversion |
| **DHT22 Microclimate** | DATA | D4 | **GPIO4** | Digital bi-directional, pulled to 3.3V via 10kΩ |
| **Relay Driver** | IN | D5 | **GPIO5** | Digital output, Active LOW trigger |
| **RGB Status LED** | Anode/Data | D21 | **GPIO21** | Digital output / PWM, current limited via 330Ω |

### C. Actuator High-Current Circuit
| Source | Source Pin | Destination | Destination Pin | Description |
| :--- | :--- | :--- | :--- | :--- |
| Relay Module | NO Terminal | 12V DC Pump | (+) Wire | Switched 12V DC power line |
| 12V Adapter | GND Bus | 12V DC Pump | (-) Wire | Continuous ground return |
| 12V DC Pump | (+) Wire | 1N4007 Diode | Cathode (Band) | Clamps positive inductive transient |
| 12V DC Pump | (-) Wire | 1N4007 Diode | Anode | Ground clamp reference |

---

## 3. High-Voltage & Inductive Isolation Rules

1. **Galvanic Isolation**:
   - The relay module features a **PC817 Optocoupler**. The logic driving the LED inside the optocoupler is powered through the ESP32 GPIO, while the relay coil is driven from the 5V rail.
2. **Flyback Suppression**:
   - The **1N4007** diode MUST be placed as close to the pump motor terminals as possible. Inductive kick occurs in $< 1\mu\text{s}$; long wiring runs between the diode and motor create EMI antennae.
3. **Star Grounding**:
   - Never daisy-chain the pump ground return wire through the breadboard sensor ground line. Route high-current motor ground directly back to the 12V power supply terminal (Star Point).
