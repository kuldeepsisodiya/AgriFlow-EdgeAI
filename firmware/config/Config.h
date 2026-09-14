/**
 * @file Config.h
 * @brief System Operating Parameters, Telemetry Intervals, and Calibration Defaults
 * @project AgriFlow-EdgeAI
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ================= BLYNK IOT CLOUD CONFIGURATION =================
// Replace placeholders with your project credentials from Blynk.Console
#define BLYNK_TEMPLATE_ID   "TMPL_AGRI_01"
#define BLYNK_TEMPLATE_NAME "AgriFlow EdgeAI"
#define BLYNK_AUTH_TOKEN    "YOUR_BLYNK_AUTH_TOKEN_HERE"

// ================= WI-FI NETWORK CONFIGURATION =================
#ifndef WIFI_SSID
#define WIFI_SSID "AgriNet_Field_2.4G"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "PrecisionAgri2026"
#endif

namespace Config {

    // ================= SENSOR CALIBRATION DEFAULTS =================
    // Raw 12-bit ADC limits (0..4095) for Capacitive Soil Moisture v1.2
    constexpr uint16_t SOIL_ADC_DRY_AIR    = 3200; // Value when suspended in dry ambient air
    constexpr uint16_t SOIL_ADC_SATURATED  = 1500; // Value submerged in water

    // Reservoir Level Sensor ADC limits
    constexpr uint16_t WATER_ADC_EMPTY     = 200;  // Dry sensor reading
    constexpr uint16_t WATER_ADC_FULL      = 2800; // Sensor submerged to top mark

    // Digital Signal Conditioning
    constexpr uint8_t MOVING_AVERAGE_WINDOW = 10;  // Number of samples for sliding window

    // ================= IRRIGATION HYSTERESIS THRESHOLDS =================
    constexpr float SOIL_DRY_THRESHOLD_PCT     = 35.0f; // Turn pump ON below this moisture
    constexpr float SOIL_WET_THRESHOLD_PCT     = 65.0f; // Turn pump OFF above this moisture
    constexpr float WATER_LEVEL_MIN_PCT        = 20.0f; // Hard interlock cutoff to prevent pump dry-run
    constexpr float WATER_LEVEL_RECOVERY_PCT   = 25.0f; // Hysteresis restart threshold for water reservoir

    // ================= HARDWARE SAFETY INTERLOCKS =================
    constexpr uint32_t PUMP_MAX_RUNTIME_MS     = 300000; // 5 Minutes maximum continuous run (prevents field flood)
    constexpr uint32_t PUMP_MANUAL_TIMEOUT_MS  = 180000; // 3 Minutes max manual override runtime
    constexpr uint32_t PUMP_MIN_COOLDOWN_MS    = 30000;  // 30 Seconds rest period before restarting pump

    // Relay Electrical Logic
    constexpr uint8_t RELAY_ACTIVE_LOGIC       = LOW;    // Active LOW relay module
    constexpr uint8_t RELAY_INACTIVE_LOGIC     = HIGH;

    // ================= COOPERATIVE TASK SCHEDULER INTERVALS =================
    constexpr uint32_t SENSOR_SAMPLE_INTERVAL_MS = 2000; // Sample physical inputs every 2 seconds
    constexpr uint32_t DECISION_EVAL_INTERVAL_MS = 2000; // Evaluate edge state machine every 2 seconds
    constexpr uint32_t BLYNK_PUSH_INTERVAL_MS   = 1000; // Stream telemetry to cloud every 1 second
    constexpr uint32_t WIFI_RECONNECT_BACKOFF_MS = 5000; // Retry Wi-Fi every 5s if disconnected

    // ================= BLYNK VIRTUAL PIN MAP =================
    namespace VPin {
        constexpr uint8_t SOIL_MOISTURE_PCT = V0;
        constexpr uint8_t PUMP_STATE        = V1;
        constexpr uint8_t WATER_LEVEL_PCT   = V2;
        constexpr uint8_t TEMPERATURE_C     = V3;
        constexpr uint8_t HUMIDITY_PCT      = V4;
        constexpr uint8_t OPERATION_MODE    = V5; // 0 = AUTO, 1 = MANUAL
        constexpr uint8_t STATUS_MESSAGE    = V6;
        constexpr uint8_t SYSTEM_UPTIME     = V7;
        constexpr uint8_t VPD_KPA           = V8;
    }

} // namespace Config

#endif // CONFIG_H
