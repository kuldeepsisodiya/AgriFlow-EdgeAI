/**
 * @file main.cpp
 * @brief AgriFlow-EdgeAI Main Firmware Orchestrator
 * @project AgriFlow-EdgeAI
 *
 * Designed for Smart India Hackathon (SIH) and Academic Minor Projects.
 * Engineered into a professional, modular, non-blocking cooperative
 * multitasking architecture.
 */

#include <Arduino.h>
#include "Config.h"
#include "PinDefinitions.h"
#include "Calibration.h"
#include "LEDManager.h"
#include "SensorManager.h"
#include "PumpController.h"
#include "WiFiManager.h"
#include "DecisionEngine.h"
#include "BlynkManager.h"

// ================= GLOBAL SUBSYSTEM INSTANCES =================
Calibration    g_calibration;
LEDManager     g_led;
SensorManager  g_sensors(g_calibration);
PumpController g_pump;
WiFiManager    g_wifi;
DecisionEngine g_decisionEngine(g_sensors, g_pump, g_led);
BlynkManager   g_blynk(g_sensors, g_pump, g_decisionEngine, g_wifi);

// ================= TASK TIMESTAMPS =================
static uint32_t s_lastSensorReadMs   = 0;
static uint32_t s_lastDecisionEvalMs = 0;
static uint32_t s_lastBlynkPushMs    = 0;
static uint32_t s_lastLogMs          = 0;

void printBanner() {
    Serial.println("\n=======================================================");
    Serial.println("         AGRIFLOW-EDGEAI PRECISION IRRIGATION         ");
    Serial.println("    SIH & Engineering Minor Project Firmware Core      ");
    Serial.println("  Modular FreeRTOS / Non-Blocking millis() Architecture");
    Serial.println("=======================================================\n");
}

void setup() {
    Serial.begin(115200);
    delay(100); // Brief UART stabilizer
    printBanner();

    // 1. Initialize Non-Volatile Storage & Transducer Profiles
    g_calibration.begin();

    // 2. Initialize Status Annunciator & Safe GPIO Pin States
    g_led.begin();
    g_pump.begin(); // Forces relay inactive immediately

    // 3. Initialize Sensor Subsystems (ADC1 & DHT22)
    g_sensors.begin();

    // 4. Initialize State Machine
    g_decisionEngine.begin();

    // 5. Initialize Network Layer (Asynchronous / Non-Blocking)
    g_wifi.begin();
    g_blynk.begin();

    Serial.println("[BOOT] All Subsystems Online. Entering Cooperative Superloop.\n");
}

void loop() {
    uint32_t now = millis();

    // ================= HIGH-FREQUENCY TASKS =================
    // Non-blocking background handlers for RF, networking, and LED animation
    g_wifi.update();
    g_blynk.update();
    g_led.update();
    g_pump.update(); // Enforces max continuous runtime watchdog

    // ================= TASK 1: SENSOR ACQUISITION (2000 ms) =================
    if (now - s_lastSensorReadMs >= Config::SENSOR_SAMPLE_INTERVAL_MS) {
        s_lastSensorReadMs = now;
        g_sensors.update();
    }

    // ================= TASK 2: DECISION ENGINE & STATE MACHINE (2000 ms) =================
    if (now - s_lastDecisionEvalMs >= Config::DECISION_EVAL_INTERVAL_MS) {
        s_lastDecisionEvalMs = now;
        g_decisionEngine.evaluate();
    }

    // ================= TASK 3: BLYNK CLOUD TELEMETRY DISPATCH (1000 ms) =================
    if (now - s_lastBlynkPushMs >= Config::BLYNK_PUSH_INTERVAL_MS) {
        s_lastBlynkPushMs = now;
        g_blynk.syncTelemetry();
    }

    // ================= TASK 4: SERIAL CONSOLE TELEMETRY LOGGER (5000 ms) =================
    if (now - s_lastLogMs >= 5000) {
        s_lastLogMs = now;
        const SensorTelemetry& t = g_sensors.getTelemetry();

        Serial.printf("[TELEMETRY] State: %-18s | Soil: %5.1f%% | Tank: %5.1f%% | Temp: %4.1fC | Hum: %4.1f%% | VPD: %.2fkPa | Pump: %s | WiFi: %s\n",
                      g_decisionEngine.getStateString(),
                      t.soilMoisturePct,
                      t.waterLevelPct,
                      t.temperatureC,
                      t.humidityPct,
                      t.vpdKpa,
                      g_pump.isRunning() ? "ON " : "OFF",
                      g_wifi.isConnected() ? "ONLINE" : "OFFLINE");
    }
}
