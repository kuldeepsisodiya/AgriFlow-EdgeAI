/**
 * @file BlynkManager.cpp
 * @brief Implementation of Blynk IoT 2.0 Cloud Integration
 * @project AgriFlow-EdgeAI
 */

#include "BlynkManager.h"

static BlynkManager* s_instance = nullptr;

// ================= BLYNK CLOUD CALLBACKS =================
// V1: Manual Pump Control Widget (Button / Switch)
BLYNK_WRITE(V1) {
    int cmd = param.asInt();
    if (s_instance) {
        Serial.printf("[BLYNK] Inbound V1 Pump Control Write: %d\n", cmd);
        // Note: Global reference will receive callback
    }
}

// V5: Operating Mode Toggle Widget (0 = AUTO, 1 = MANUAL)
BLYNK_WRITE(V5) {
    int modeVal = param.asInt();
    if (s_instance) {
        Serial.printf("[BLYNK] Inbound V5 Mode Toggle Write: %d\n", modeVal);
    }
}

BlynkManager::BlynkManager(SensorManager& sensors, PumpController& pump, DecisionEngine& engine, WiFiManager& wifi)
    : _sensors(sensors),
      _pump(pump),
      _engine(engine),
      _wifi(wifi),
      _lastSyncMs(0),
      _blynkConfigured(false) {
    s_instance = this;
}

void BlynkManager::begin() {
    // Check if user has populated placeholder credentials
    if (strcmp(BLYNK_AUTH_TOKEN, "YOUR_BLYNK_AUTH_TOKEN_HERE") == 0) {
        Serial.println("[BLYNK] NOTE: Default placeholder token active. Cloud synchronization deferred.");
        _blynkConfigured = false;
        return;
    }

    Blynk.config(BLYNK_AUTH_TOKEN);
    _blynkConfigured = true;
    Serial.println("[BLYNK] Initialized with template credentials.");
}

void BlynkManager::update() {
    if (!_blynkConfigured || !_wifi.isConnected()) {
        return; // Prevent blocking when offline
    }

    Blynk.run();
}

void BlynkManager::syncTelemetry() {
    if (!_blynkConfigured || !Blynk.connected()) {
        return;
    }

    const SensorTelemetry& data = _sensors.getTelemetry();

    // Push physical metrics
    Blynk.virtualWrite(Config::VPin::SOIL_MOISTURE_PCT, data.soilMoisturePct);
    Blynk.virtualWrite(Config::VPin::PUMP_STATE,        _pump.isRunning() ? 1 : 0);
    Blynk.virtualWrite(Config::VPin::WATER_LEVEL_PCT,   data.waterLevelPct);
    Blynk.virtualWrite(Config::VPin::TEMPERATURE_C,     data.temperatureC);
    Blynk.virtualWrite(Config::VPin::HUMIDITY_PCT,      data.humidityPct);
    Blynk.virtualWrite(Config::VPin::OPERATION_MODE,    static_cast<int>(_engine.getMode()));
    Blynk.virtualWrite(Config::VPin::STATUS_MESSAGE,    _engine.getStateString());
    Blynk.virtualWrite(Config::VPin::SYSTEM_UPTIME,     millis() / 1000);
    Blynk.virtualWrite(Config::VPin::VPD_KPA,           data.vpdKpa);
}

void BlynkManager::triggerAlarm(const char* eventName, const char* description) {
    if (_blynkConfigured && Blynk.connected()) {
        Blynk.logEvent(eventName, description);
        Serial.printf("[BLYNK] Push Alarm Sent: Event '%s' -> %s\n", eventName, description);
    }
}
