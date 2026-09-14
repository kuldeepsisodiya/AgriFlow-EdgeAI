# Cloud & Edge Telemetry Architecture

This document specifies the end-to-end communication protocols, cloud datastream mappings, and edge resilience mechanisms for **AgriFlow-EdgeAI**.

---

## 1. Multi-Tier IoT Topology

```text
┌────────────────────────────────────────────────────────┐
│           CLOUD & USER APPLICATION TIER                │
│    Blynk IoT 2.0 Cloud Engine | TLS 1.3 Streaming      │
│    Web Dashboard | Android/iOS App | Push Notifications│
└───────────────────────────▲────────────────────────────┘
                            │ Bi-Directional Virtual Pins (V0-V8)
                            │ 802.11 b/g/n Wi-Fi (WPA2)
┌───────────────────────────▼────────────────────────────┐
│              ESP32 EDGE COMPUTING CORE                 │
│    Cooperative millis() Event Loop | FreeRTOS Base     │
│    Moving Average Filter | Hysteresis Decision Engine  │
│    NVS Profile Storage | Anti-Cavitation Safety Lock   │
└───────────────────────────▲────────────────────────────┘
                            │ GPIO / ADC1 Channels
┌───────────────────────────▼────────────────────────────┐
│           PERCEPTION & ACTUATION FIELD LAYER           │
│    Capacitive Soil Probe | Immersion Reservoir Sensor  │
│    DHT22 Microclimate | 5V Optocoupled Relay | 12V Pump│
└────────────────────────────────────────────────────────┘
```

---

## 2. Blynk IoT 2.0 Datastream Dictionary

The system communicates with the Blynk IoT 2.0 cloud broker via encrypted TCP/IP sockets. The table below outlines the registered datastreams:

| Virtual Pin | Datastream Name | Data Type | Range | Units | Access Mode | Description |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **V0** | `Soil_Moisture` | Double | `0.0 – 100.0` | `%` | Read | Calibrated Volumetric Water Content |
| **V1** | `Pump_Control` | Integer | `0 or 1` | `—` | Read / Write | Remote Actuator Command & Relay Feedback |
| **V2** | `Water_Level` | Double | `0.0 – 100.0` | `%` | Read | Reservoir Water Depth Percentage |
| **V3** | `Temperature` | Double | `-40.0 – 80.0`| `°C` | Read | Ambient Air Temperature from DHT22 |
| **V4** | `Humidity` | Double | `0.0 – 100.0` | `%` | Read | Ambient Relative Humidity from DHT22 |
| **V5** | `Operation_Mode`| Integer | `0 or 1` | `—` | Read / Write | `0 = AUTOMATIC`, `1 = MANUAL OVERRIDE` |
| **V6** | `System_State` | String | `Max 32 Chars`| `—` | Read | Live FSM State (`MONITORING_IDLE`, etc.) |
| **V7** | `System_Uptime` | Integer | `0 – 4294967` | `s` | Read | Microcontroller Continuous Run Seconds |
| **V8** | `VPD_Deficit` | Double | `0.0 – 10.0` | `kPa` | Read | Atmospheric Vapor Pressure Deficit |

---

## 3. Edge Autonomy & Network Partition Resilience

A critical requirement of the **Smart India Hackathon** evaluation is field survivability during rural telecom outages.

### Failure Scenario: Complete Wi-Fi Loss
1. **Zero Thread-Blocking**:
   - `WiFiManager` detects link drop via non-blocking status polling.
   - `BlynkManager.update()` is immediately bypassed, ensuring the CPU never blocks waiting for socket handshakes.
2. **Local Closed-Loop Autonomy**:
   - `SensorManager` continues sampling physical ADC channels every 2000 ms.
   - `DecisionEngine` evaluates soil moisture against the 35% / 65% hysteresis boundaries locally.
   - The pump safely engages, water is dispensed, and the pump shuts off when target hydration is attained.
3. **Safety Interlocks Remain Active**:
   - The reservoir anti-cavitation lock and 300-second maximum runtime watchdog operate strictly on hardware timestamps in local SRAM, completely decoupled from cloud connectivity.
4. **Seamless Reconnection**:
   - `WiFiManager` attempts periodic background re-association every 5000 ms.
   - Upon network restoration, TLS handshake completes automatically, and pending telemetry synchronizes with the Blynk cloud dashboard without requiring a device reset.
