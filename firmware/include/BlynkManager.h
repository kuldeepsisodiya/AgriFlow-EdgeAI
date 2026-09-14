/**
 * @file BlynkManager.h
 * @brief Blynk IoT 2.0 Telemetry Synchronization & Cloud Remote Control
 * @project AgriFlow-EdgeAI
 */

#ifndef BLYNK_MANAGER_H
#define BLYNK_MANAGER_H

#include <Arduino.h>
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include "Config.h"
#include <BlynkSimpleEsp32.h>

#include "SensorManager.h"
#include "PumpController.h"
#include "DecisionEngine.h"
#include "WiFiManager.h"

class BlynkManager {
public:
    BlynkManager(SensorManager& sensors, PumpController& pump, DecisionEngine& engine, WiFiManager& wifi);

    /**
     * @brief Initialize Blynk credentials and begin connection
     */
    void begin();

    /**
     * @brief Servicing loop for inbound cloud packets
     */
    void update();

    /**
     * @brief Transmit fresh sensor telemetry and system state to cloud virtual pins
     */
    void syncTelemetry();

    /**
     * @brief Dispatch in-app push alert notification
     */
    void triggerAlarm(const char* eventName, const char* description);

private:
    SensorManager&  _sensors;
    PumpController& _pump;
    DecisionEngine& _engine;
    WiFiManager&    _wifi;

    uint32_t        _lastSyncMs;
    bool            _blynkConfigured;
};

#endif // BLYNK_MANAGER_H
