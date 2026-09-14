# AgriFlow-EdgeAI — Embedded Firmware

This directory contains the production-grade, modular C++ firmware for the **AgriFlow-EdgeAI** system, engineered for the ESP32 microcontroller using PlatformIO and the Arduino core.

---

## Firmware Architecture

Unlike conventional hobbyist tutorials that lump all logic, blocking delays, and sensor reads into a single monolithic `.ino` file, this firmware is architected into 8 decoupled, testable subsystems:

```text
firmware/
├── platformio.ini              # PlatformIO environment, compiler flags, and library dependencies
├── config/
│   ├── Config.h                # System thresholds, timing intervals, and Blynk virtual pin registry
│   └── PinDefinitions.h        # Hardware GPIO mappings with electrical justifications
├── include/
│   ├── BlynkManager.h          # Bi-directional IoT cloud sync & remote commands
│   ├── Calibration.h           # Non-Volatile Storage (NVS) transducer profiles
│   ├── DecisionEngine.h        # Edge finite state machine (FSM) and hysteresis logic
│   ├── LEDManager.h            # Non-blocking visual heartbeat and alarm status
│   ├── PumpController.h        # Relay driver with anti-cavitation and runtime watchdogs
│   ├── SensorManager.h         # ADC1 10-sample moving average filter & DHT22 driver
│   └── WiFiManager.h           # Asynchronous Wi-Fi connection with auto-reconnect backoff
└── src/
    ├── BlynkManager.cpp
    ├── Calibration.cpp
    ├── DecisionEngine.cpp
    ├── LEDManager.cpp
    ├── PumpController.cpp
    ├── SensorManager.cpp
    ├── WiFiManager.cpp
    └── main.cpp                # Clean cooperative task dispatcher (0ms blocking delay)
```

---

## Key Firmware Features

1. **Non-Blocking Execution (`millis()` Scheduler)**:
   - Zero use of blocking `delay()` in recurrent operations.
   - Microsecond cooperative multitasking ensures continuous sensor reads and prompt watchdog responses.
2. **Moving Average Filter**:
   - 10-sample circular sliding window on analog channels cancels high-frequency noise and suppresses spikes caused by relay coil energization.
3. **Dual Hysteresis Deadband**:
   - Soil moisture trigger at `35%` (dry) and cutoff at `65%` (wet) prevents rapid relay contact chattering.
4. **Hardware Interlocks**:
   - Hard dry-run lockout if reservoir level `< 20%`.
   - Maximum continuous runtime watchdog (`300 seconds`) prevents field flooding in the event of pipe rupture or sensor displacement.
5. **Offline Autonomous Resilience**:
   - If Wi-Fi is lost, the device automatically falls back to local edge control and continues autonomous irrigation without crashing.

---

## Build & Flash Instructions

### Prerequisites
- [PlatformIO Core (CLI)](https://docs.platformio.org/en/latest/core/installation/index.html) or [PlatformIO IDE for VSCode](https://platformio.org/platformio-for-vscode).

### 1. Configure Credentials
Open [`config/Config.h`](file:///Users/kuldeep/Documents/GitHub/AgriFlow-EdgeAI/firmware/config/Config.h) and enter your local Wi-Fi and Blynk credentials:
```cpp
#define BLYNK_TEMPLATE_ID   "TMPL_YOUR_ID"
#define BLYNK_DEVICE_NAME   "AgriFlow EdgeAI"
#define BLYNK_AUTH_TOKEN    "YOUR_TOKEN_HERE"

#define WIFI_SSID           "Your_WiFi_SSID"
#define WIFI_PASSWORD       "Your_WiFi_Password"
```

### 2. Build the Firmware
```bash
pio run
```

### 3. Upload to ESP32
Connect your ESP32 board via USB (CP2102 or CH340 driver):
```bash
pio run --target upload
```

### 4. Monitor Serial Output
```bash
pio device monitor --baud 115200
```
