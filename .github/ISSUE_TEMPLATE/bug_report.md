---
name: Bug Report
about: Create a report to help us improve AgriFlow-EdgeAI firmware or hardware integration
title: "[BUG]: "
labels: ["bug", "triage"]
assignees: ''

---

**Describe the Bug**
A clear and concise description of what the bug is.

**Hardware Configuration**
- **Microcontroller**: ESP32 DevKit V1 (ESP-WROOM-32) / Other:
- **Power Supply**: 5V/2A USB / 12V 2A Adapter with Buck Converter
- **Soil Moisture Sensor**: Capacitive v1.2 / Resistive
- **Water Level Sensor**: Resistive contact / Float switch / Ultrasonic
- **Relay**: 5V Optocoupler-isolated (Active Low / Active High)
- **Pump**: 12V DC Submersible / R385 Diaphragm

**Firmware Version & Environment**
- Firmware Version: (e.g. v1.0.0)
- PlatformIO Core Version:
- Arduino ESP32 Core Version:

**To Reproduce**
Steps to reproduce the behavior:
1. Boot ESP32 with sensor reading `...`
2. Wait for `...`
3. Trigger condition `...`
4. See error in Serial Monitor

**Expected Behavior**
A clear and concise description of what you expected to happen.

**Serial Monitor Output (115200 Baud)**
```text
Paste serial monitor logs here
```

**Additional Context**
Add any other context about the problem here (e.g. ambient temperature, soil salinity, Wi-Fi router distance).
