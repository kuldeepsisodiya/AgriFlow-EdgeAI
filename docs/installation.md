# Installation & Quickstart Deployment Guide

This document provides a step-by-step walk-through for assembling hardware, setting up the Blynk IoT 2.0 Cloud dashboard, and compiling/flashing the **AgriFlow-EdgeAI** firmware.

---

## 1. Hardware Assembly Checklist

1. **Power Supply Setup**:
   - Connect the 12V 2A DC adapter to the `IN+` and `IN-` terminals of the LM2596 buck converter.
   - Using a digital multimeter, measure the voltage across `OUT+` and `OUT-`. Adjust the multi-turn trimpot until the output reads exactly **5.05V DC**.
   - Disconnect power before wiring to the ESP32.
2. **ESP32 & Sensors Interconnect**:
   - Connect buck converter `OUT+` (5.05V) to ESP32 `VIN`.
   - Connect buck converter `OUT-` (GND) to ESP32 `GND`.
   - Wire Capacitive Soil Moisture Sensor `AOUT` to **GPIO35**.
   - Wire Water Reservoir Sensor `AOUT` to **GPIO34**.
   - Wire DHT22 `DATA` pin to **GPIO4** (insert 10kΩ pull-up between DATA and 3.3V).
   - Wire Relay `IN` pin to **GPIO5**.
   - Solder the **1N4007 flyback diode** across the 12V pump leads in reverse bias (cathode band to pump positive lead).

---

## 2. Setting Up Blynk IoT 2.0 Cloud

### Step 1: Create a Device Template
1. Navigate to [Blynk.Console](https://blynk.cloud) and sign in.
2. Click **Developer Zone** $\rightarrow$ **Templates** $\rightarrow$ **+ New Template**.
3. Name: `AgriFlow EdgeAI` | Hardware: `ESP32` | Connection Type: `WiFi`.

### Step 2: Configure Virtual Datastreams
Under the **Datastreams** tab, create the following 9 datastreams:

| Datastream Name | Pin | Data Type | Min | Max | Default |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `Soil_Moisture` | **V0** | Double | 0 | 100 | 0 |
| `Pump_Control` | **V1** | Integer | 0 | 1 | 0 |
| `Water_Level` | **V2** | Double | 0 | 100 | 0 |
| `Temperature` | **V3** | Double | -40 | 80 | 0 |
| `Humidity` | **V4** | Double | 0 | 100 | 0 |
| `Operation_Mode`| **V5** | Integer | 0 | 1 | 0 |
| `System_State` | **V6** | String | — | — | `BOOT` |
| `System_Uptime` | **V7** | Integer | 0 | 4294967 | 0 |
| `VPD_Deficit` | **V8** | Double | 0 | 10 | 0 |

### Step 3: Design Dashboard Widgets
- **Mobile App**:
  - Add two **Radial Gauges** mapped to `V0` (Soil Moisture %) and `V2` (Water Level %).
  - Add a **Styled Button** mapped to `V1` for manual pump override.
  - Add a **Segmented Switch** mapped to `V5` for Auto / Manual mode toggling.
  - Add **Value Displays** for `V3` (Temperature) and `V4` (Humidity).
- **Web Dashboard**:
  - Add a **SuperChart** widget plotting `V0`, `V2`, `V3`, and `V4` over a 24-hour window.

### Step 4: Obtain Authentication Token
In the **Search** $\rightarrow$ **Devices** tab, click **+ New Device** $\rightarrow$ **From Template**. Copy the generated `BLYNK_AUTH_TOKEN`.

---

## 3. Firmware Compilation & Flashing

### Step 1: Clone the Repository
```bash
git clone https://github.com/kuldeepsisodiya/AgriFlow-EdgeAI.git
cd AgriFlow-EdgeAI/firmware
```

### Step 2: Configure Secrets
Open [`config/Config.h`](file:///Users/kuldeep/Documents/GitHub/AgriFlow-EdgeAI/firmware/config/Config.h) and update your network parameters:
```cpp
#define BLYNK_TEMPLATE_ID   "TMPL_YOUR_ID"
#define BLYNK_TEMPLATE_NAME "AgriFlow EdgeAI"
#define BLYNK_AUTH_TOKEN    "YOUR_COPIED_TOKEN"

#define WIFI_SSID           "Your_WiFi_SSID"
#define WIFI_PASSWORD       "Your_WiFi_Password"
```

### Step 3: Build & Flash via PlatformIO
Connect your ESP32 to your workstation using a micro-USB/USB-C data cable:
```bash
# Verify compilation
pio run

# Flash to ESP32
pio run --target upload

# Open Serial Telemetry Stream at 115200 Baud
pio device monitor --baud 115200
```

---

## 4. Benchtop Verification Protocol

Verify successful bringup through the serial monitor logs:
1. `[CALIBRATION] Loaded NVS Profile -> SoilDry: 3200, SoilWet: 1500`
2. `[PUMP_CONTROLLER] Relay initialized to SAFE (HIGH/DE-ENERGIZED).`
3. `[SENSOR_MANAGER] ADC1 (GPIO34, GPIO35) and DHT22 Initialized.`
4. `[DECISION_ENGINE] State Machine transitioned to MONITORING_IDLE (Mode: AUTOMATIC).`
5. `[WIFI_MANAGER] CONNECTED! Local IP: 192.168.1.xxx`
6. `[TELEMETRY] State: MONITORING_IDLE | Soil: 28.5% | Tank: 84.0% | Temp: 28.2C | Pump: OFF`
