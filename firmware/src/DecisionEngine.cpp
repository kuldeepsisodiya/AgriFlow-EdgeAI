/**
 * @file DecisionEngine.cpp
 * @brief Implementation of Decision Engine and FSM
 * @project AgriFlow-EdgeAI
 */

#include "DecisionEngine.h"

DecisionEngine::DecisionEngine(SensorManager& sensors, PumpController& pump, LEDManager& led)
    : _sensors(sensors),
      _pump(pump),
      _led(led),
      _mode(OperationMode::AUTOMATIC),
      _state(SystemState::BOOT_INIT),
      _manualPumpRequest(false),
      _manualStartMs(0) {}

void DecisionEngine::begin() {
    _state = SystemState::MONITORING_IDLE;
    updateVisualIndicator();
    Serial.println("[DECISION_ENGINE] State Machine transitioned to MONITORING_IDLE (Mode: AUTOMATIC).");
}

void DecisionEngine::setMode(OperationMode mode) {
    if (_mode != mode) {
        _mode = mode;
        Serial.printf("[DECISION_ENGINE] Mode switched to: %s\n",
                      (mode == OperationMode::AUTOMATIC) ? "AUTOMATIC" : "MANUAL");

        // When switching back to AUTOMATIC, immediately cancel pending manual requests
        if (_mode == OperationMode::AUTOMATIC && _pump.isRunning()) {
            _pump.stop();
        }
    }
}

void DecisionEngine::setManualPumpCommand(bool engage) {
    _manualPumpRequest = engage;
    if (engage) {
        _manualStartMs = millis();
    }
}

void DecisionEngine::evaluate() {
    const SensorTelemetry& telemetry = _sensors.getTelemetry();

    // ================= GLOBAL SAFETY INTERLOCK 1: SENSOR FAULT =================
    if (telemetry.sensorFault) {
        if (_state != SystemState::SENSOR_FAULT_FAILSAFE) {
            Serial.println("[DECISION_ENGINE] FAULT: Sensor acquisition out of range! Locking pump.");
            _pump.emergencyStop();
            _state = SystemState::SENSOR_FAULT_FAILSAFE;
            updateVisualIndicator();
        }
        return;
    }

    // ================= GLOBAL SAFETY INTERLOCK 2: RESERVOIR LEVEL =================
    if (telemetry.waterLevelPct < Config::WATER_LEVEL_MIN_PCT) {
        if (_state != SystemState::TANK_EMPTY_LOCKED) {
            Serial.printf("[DECISION_ENGINE] ALARM: Reservoir critical (%.1f%% < %.1f%%)! Locking pump.\n",
                          telemetry.waterLevelPct, Config::WATER_LEVEL_MIN_PCT);
            _pump.stop();
            _state = SystemState::TANK_EMPTY_LOCKED;
            updateVisualIndicator();
        }
        return;
    } else if (_state == SystemState::TANK_EMPTY_LOCKED) {
        // Recovery hysteresis: Require tank to fill past RECOVERY threshold before clearing
        if (telemetry.waterLevelPct >= Config::WATER_LEVEL_RECOVERY_PCT) {
            Serial.printf("[DECISION_ENGINE] RECOVERY: Reservoir refilled (%.1f%% >= %.1f%%). Resuming.\n",
                          telemetry.waterLevelPct, Config::WATER_LEVEL_RECOVERY_PCT);
            _state = SystemState::MONITORING_IDLE;
            updateVisualIndicator();
        } else {
            return; // Still waiting for full recovery
        }
    }

    // ================= MODE DISPATCH =================
    if (_mode == OperationMode::AUTOMATIC) {
        handleAutomaticMode(telemetry);
    } else {
        handleManualMode(telemetry);
    }

    updateVisualIndicator();
}

void DecisionEngine::handleAutomaticMode(const SensorTelemetry& telemetry) {
    float moisture = telemetry.soilMoisturePct;

    if (_pump.isRunning()) {
        _state = SystemState::AUTO_WATERING;

        // Hysteresis Upper Boundary: Stop watering once target moisture is achieved
        if (moisture >= Config::SOIL_WET_THRESHOLD_PCT) {
            Serial.printf("[DECISION_ENGINE] Moisture target reached (%.1f%% >= %.1f%%). Disengaging pump.\n",
                          moisture, Config::SOIL_WET_THRESHOLD_PCT);
            _pump.stop();
            _state = SystemState::MONITORING_IDLE;
        }
    } else {
        // Hysteresis Lower Boundary: Engage pump when soil drops below deficit threshold
        if (moisture < Config::SOIL_DRY_THRESHOLD_PCT) {
            Serial.printf("[DECISION_ENGINE] Soil moisture deficit detected (%.1f%% < %.1f%%). Starting pump.\n",
                          moisture, Config::SOIL_DRY_THRESHOLD_PCT);
            if (_pump.start(telemetry.waterLevelPct)) {
                _state = SystemState::AUTO_WATERING;
            }
        } else {
            _state = SystemState::MONITORING_IDLE;
        }
    }
}

void DecisionEngine::handleManualMode(const SensorTelemetry& telemetry) {
    if (_manualPumpRequest) {
        // Enforce maximum continuous manual runtime timeout
        if (millis() - _manualStartMs >= Config::PUMP_MANUAL_TIMEOUT_MS) {
            Serial.println("[DECISION_ENGINE] Manual override timeout reached (3 min). Auto-clearing command.");
            _manualPumpRequest = false;
            _pump.stop();
            _state = SystemState::MONITORING_IDLE;
            return;
        }

        if (!_pump.isRunning()) {
            if (_pump.start(telemetry.waterLevelPct)) {
                _state = SystemState::MANUAL_OVERRIDE;
            } else {
                _manualPumpRequest = false;
            }
        } else {
            _state = SystemState::MANUAL_OVERRIDE;
        }
    } else {
        if (_pump.isRunning()) {
            _pump.stop();
        }
        _state = SystemState::MONITORING_IDLE;
    }
}

void DecisionEngine::updateVisualIndicator() {
    switch (_state) {
        case SystemState::BOOT_INIT:
            _led.setPattern(VisualPattern::CONNECTING);
            break;
        case SystemState::MONITORING_IDLE:
            _led.setPattern(VisualPattern::IDLE_HEALTHY);
            break;
        case SystemState::AUTO_WATERING:
        case SystemState::MANUAL_OVERRIDE:
            _led.setPattern(VisualPattern::IRRIGATING);
            break;
        case SystemState::TANK_EMPTY_LOCKED:
        case SystemState::SENSOR_FAULT_FAILSAFE:
            _led.setPattern(VisualPattern::ERROR_ALERT);
            break;
    }
}

const char* DecisionEngine::getStateString() const {
    switch (_state) {
        case SystemState::BOOT_INIT:             return "BOOT_INIT";
        case SystemState::MONITORING_IDLE:       return "MONITORING_IDLE";
        case SystemState::AUTO_WATERING:         return "AUTO_WATERING";
        case SystemState::MANUAL_OVERRIDE:       return "MANUAL_OVERRIDE";
        case SystemState::TANK_EMPTY_LOCKED:     return "TANK_EMPTY_LOCKED";
        case SystemState::SENSOR_FAULT_FAILSAFE: return "SENSOR_FAULT_FAILSAFE";
        default:                                 return "UNKNOWN";
    }
}
