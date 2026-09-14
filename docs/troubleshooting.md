# Engineering Troubleshooting & Diagnostic Matrix

This guide provides diagnostic procedures and root-cause solutions for electrical, embedded firmware, and cloud anomalies in **AgriFlow-EdgeAI**.

---

## 1. Electrical & Power Anomalies

| Symptom / Error | Root Cause | Engineering Solution |
| :--- | :--- | :--- |
| **`Brownout detector was triggered` / ESP32 reboots continuously** | 5V rail voltage dips below 2.7V due to current inrush when the 12V pump motor starts. | 1. Ensure the 12V adapter is rated for $\ge 2.0\text{ A}$.<br>2. Solder a $100\mu\text{F}$ electrolytic capacitor across the LM2596 buck converter `OUT+`/`OUT-` terminals.<br>3. Verify LM2596 output is tuned to $5.05\text{V}$ under load. |
| **ESP32 freezes or reboots the instant relay clicks** | Inductive coil kickback ($V = -L \frac{di}{dt}$) creates a high-voltage transient propagating back through common logic ground. | 1. Verify reverse-bias **1N4007 flyback diode** is soldered across pump terminals.<br>2. Ensure relay optocoupler jumper (`JD-VCC`) is powered from 5V buck rail, not the ESP32 3.3V pin.<br>3. Implement star-grounding (do not route motor ground through breadboard). |
| **Microcontroller warm to touch / excessive current** | External transducer powered from ESP32 3.3V pin exceeding AMS1117 LDO thermal limits. | Power the 5V relay module and sensors from the LM2596 5V rail instead of the onboard 3.3V regulator pin. |

---

## 2. Transducer & Signal Conditioning Anomalies

| Symptom / Error | Root Cause | Engineering Solution |
| :--- | :--- | :--- |
| **Soil moisture stuck at 0% (or raw ADC reads 4095)** | Sensor unplugged, broken ground wire, or pin misconfigured. | 1. Check wiring to `GPIO35`. Verify sensor VCC has 3.3V–5.0V.<br>2. Inspect capacitive probe solder joints.<br>3. Verify calibration limits in NVS (`SOIL_ADC_DRY_AIR`). |
| **Soil moisture stuck at 100% (or raw ADC reads ~1000)** | Short circuit on signal line or water entered sensor circuitry. | 1. Inspect capacitive sensor head. If water penetrated the unsealed top components, dry thoroughly with isopropyl alcohol and seal with silicone conformal coating.<br>2. Re-run two-point calibration. |
| **DHT22 returns `nan` (Not a Number) or read timeout** | Missing pull-up resistor or cable run $> 2\text{ meters}$ without level shifting. | 1. Ensure a **10kΩ resistor** connects `GPIO4` to `3V3`.<br>2. Ensure polling rate does not exceed once every 2 seconds (DHT22 sensor recovery limit). |
| **Erratic analog sensor jumps whenever Wi-Fi transmits** | Sensor connected to an **ADC2** pin (GPIO 0, 2, 4, 12–15, 25–27). | Move sensor to **ADC1** pins (`GPIO34`, `GPIO35`, `GPIO36`, or `GPIO39`). In AgriFlow-EdgeAI, soil is dedicated to `GPIO35` and water level to `GPIO34`. |

---

## 3. Actuator & Logic Control Anomalies

| Symptom / Error | Root Cause | Engineering Solution |
| :--- | :--- | :--- |
| **Pump will not start even though soil moisture is dry (< 35%)** | 1. Reservoir level $< 20.0\%$ (Anti-cavitation lock).<br>2. Pump in 30-second thermal cooldown.<br>3. Active-HIGH vs Active-LOW inversion. | 1. Check reservoir level gauge. If $< 20\%$, refill tank to $> 25\%$.<br>2. Inspect Serial Monitor: check if `[PUMP_CONTROLLER] REJECTED: Low Reservoir Level` is logged.<br>3. Verify relay logic level in `Config.h` matches module (`LOW = active`). |
| **Relay clicks ON and immediately OFF every few seconds** | Single threshold without hysteresis deadband. | Ensure firmware uses dual hysteresis thresholds (`SOIL_DRY = 35%`, `SOIL_WET = 65%`). Verify `MOVING_AVERAGE_WINDOW = 10` is active. |
| **Pump cuts off after exactly 5 minutes of watering** | Maximum continuous runtime watchdog (`PUMP_MAX_RUNTIME_MS = 300000`) triggered. | Safety feature functioning normally to prevent field flooding. Check water supply, pipe leaks, or whether sensor is positioned in the irrigated root zone. |

---

## 4. Network & Cloud Telemetry Anomalies

| Symptom / Error | Root Cause | Engineering Solution |
| :--- | :--- | :--- |
| **Wi-Fi refuses to associate / stays offline** | 1. Attempting connection to 5 GHz Wi-Fi band.<br>2. Weak RSSI ($< -80\text{ dBm}$). | 1. ESP32 hardware supports **2.4 GHz only**. Configure router to broadcast a dedicated 2.4 GHz SSID.<br>2. Check antenna proximity to metal enclosures. |
| **Blynk reports "Device Offline" but Wi-Fi connected** | Incorrect `BLYNK_TEMPLATE_ID` or invalid `BLYNK_AUTH_TOKEN`. | Copy credentials directly from the Blynk Console Device info tab and update [`config/Config.h`](file:///Users/kuldeep/Documents/GitHub/AgriFlow-EdgeAI/firmware/config/Config.h). |
| **Firmware halts or freezes when Wi-Fi drops** | Blocking Wi-Fi reconnect code in `loop()`. | AgriFlow-EdgeAI utilizes non-blocking `WiFiManager` with exponential backoff. Do not call blocking `WiFi.waitForConnectResult()` in the recurrent loop. |
