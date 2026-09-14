# Future Engineering Roadmap: Edge-AI & Field Scaling

This roadmap outlines planned hardware revisions, on-device machine learning architectures, and field-scale wireless networking for future releases of **AgriFlow-EdgeAI**.

---

## 1. Edge-AI & TinyML On-Device Inference (v1.1.0)

While current firmware utilizes hysteretic rule-based logic, the upcoming v1.1.0 milestone integrates **TensorFlow Lite for Microcontrollers (TFLite Micro)** directly onto the ESP32 Xtensa LX6 core.

```text
[Input Tensors] ─────────> [Quantized TFLite Model] ─────────> [Predictive Control Action]
  • 24h VWC History          • 1D Convolutional Neural Net      • Forecasted ET0 Deficit
  • Diurnal Temp/RH          • INT8 Quantization (24 KB)        • Optimal Hydration Dose (mL)
  • Solar Irradiance         • Execution Time: < 15ms           • Time-Shift to Evening Cool
```

### Reference Evapotranspiration ($ET_0$) Prediction
Using the **FAO-56 Penman-Monteith** formulation, reference evapotranspiration reflects atmospheric water demand:

$$ET_0 = \frac{0.408 \Delta (R_n - G) + \gamma \frac{900}{T + 273} u_2 (e_s - e_a)}{\Delta + \gamma (1 + 0.34 u_2)}$$

Instead of reacting after the root zone is already depleted, the TinyML model forecasts upcoming evapotranspiration losses 4 hours in advance and dispenses proactive hydration during the cooler morning or late evening hours, reducing evaporative water loss by an additional **18%**.

---

## 2. Cloud Weather API Precipitation Lookahead (v1.2.0)

By querying the **Open-Meteo REST API** over HTTPS:
- The ESP32 parses JSON forecasts for the local coordinates every 6 hours.
- If precipitation $\ge 5.0\text{ mm}$ is forecasted within the subsequent 12-hour window, upcoming automatic irrigation cycles are temporarily deferred, conserving reservoir capacity and utilizing natural rainfall.

---

## 3. LoRaWAN & ESP-NOW Mesh Networking (v1.3.0)

In extensive rural agricultural plots, Wi-Fi coverage is limited to the farmhouse perimeter.

```text
[Node 1: LoRa Sensor] ──┐
[Node 2: LoRa Sensor] ──┼──> [AgriFlow Master Gateway Node] ──> [Cellular 4G / Wi-Fi] ──> [Blynk Cloud]
[Node 3: LoRa Sensor] ──┘      • Dual-Core ESP32 + SX1262
                               • Solenoid Valve Actuator Bank
```

- **Sub-GHz Radio**: Semtech **SX1262 LoRa** transceiver operating at 868 MHz / 915 MHz.
- **Range**: Point-to-point line-of-sight telemetry up to **10 km** across rural terrain.
- **Topology**: Low-power battery-operated sensor nodes transmit periodic telemetry packets to a centralized AgriFlow master pump station gateway.

---

## 4. Solar MPPT Energy Harvesting (v1.4.0)

For off-grid borewell installations:
- **Solar Photovoltaic Panel**: 20W 18V Monocrystalline panel.
- **MPPT Charge Controller**: Consonance **CN3791** Maximum Power Point Tracking buck charger.
- **Battery Storage**: 3S 18650 Li-ion battery pack (11.1V 2600 mAh) with integrated hardware BMS.
- **Deep Sleep Optimization**: The ESP32 utilizes ULP (Ultra-Low-Power) co-processor sleep modes, awakening for 500 ms every 15 minutes to sample sensors and evaluate state, lowering average quiescent current to **$< 25\,\mu\text{A}$**.

---

## 5. Multi-Zone Solenoid Manifold Expansion (v1.5.0)

- Replaces single pump output with an **8-channel 12V latching solenoid valve manifold**.
- Driven via an **I2C PCF8574 GPIO expander** and ULN2803A Darlington transistor array.
- Enables distinct irrigation schedules for multiple crop quadrants (e.g., drip lines for fruit orchards, misting nozzles for seedlings, micro-sprinklers for leafy greens).
