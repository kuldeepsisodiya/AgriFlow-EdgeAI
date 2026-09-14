/**
 * @file PumpController.cpp
 * @brief Implementation of Pump Controller and Safety Interlocks
 * @project AgriFlow-EdgeAI
 */

#include "PumpController.h"

PumpController::PumpController()
    : _state(PumpState::OFF),
      _startTimeMs(0),
      _stopTimeMs(0) {}

void PumpController::begin() {
    // CRITICAL SAFETY PATTERN: Write inactive state to output register BEFORE setting pinMode
    // to prevent any millivolt floating glitch from momentary coil activation at boot
    digitalWrite(Pins::RELAY_PUMP, Config::RELAY_INACTIVE_LOGIC);
    pinMode(Pins::RELAY_PUMP, OUTPUT);
    digitalWrite(Pins::RELAY_PUMP, Config::RELAY_INACTIVE_LOGIC);

    _state = PumpState::OFF;
    Serial.println("[PUMP_CONTROLLER] Relay initialized to SAFE (HIGH/DE-ENERGIZED).");
}

void PumpController::update() {
    if (_state == PumpState::RUNNING) {
        uint32_t runtime = millis() - _startTimeMs;

        // Watchdog 1: Maximum Continuous Runtime Cutoff (Flooding Prevention)
        if (runtime >= Config::PUMP_MAX_RUNTIME_MS) {
            Serial.printf("[PUMP_CONTROLLER] ALARM: Max Continuous Runtime (%u ms) Exceeded! Force Disengaging.\n", runtime);
            setRelay(false);
            _state = PumpState::TRIPPED_MAX_RUNTIME;
            _stopTimeMs = millis();
        }
    } else if (_state == PumpState::TRIPPED_MAX_RUNTIME || _state == PumpState::IN_COOLDOWN) {
        // Clear cooldown lock after minimum rest period
        if (millis() - _stopTimeMs >= Config::PUMP_MIN_COOLDOWN_MS) {
            _state = PumpState::OFF;
            Serial.println("[PUMP_CONTROLLER] Cooldown period expired. Safety lock cleared.");
        }
    }
}

bool PumpController::start(float reservoirLevelPct) {
    // Interlock 1: Dry-Run Protection (Reservoir must be above minimum threshold)
    if (reservoirLevelPct < Config::WATER_LEVEL_MIN_PCT) {
        Serial.printf("[PUMP_CONTROLLER] REJECTED: Low Reservoir Level (%.1f%% < %.1f%%). Anti-Cavitation Guard Active!\n",
                      reservoirLevelPct, Config::WATER_LEVEL_MIN_PCT);
        _state = PumpState::TRIPPED_DRY_RUN;
        return false;
    }

    // Interlock 2: Enforce Cooldown Deadband
    if (_state == PumpState::IN_COOLDOWN || _state == PumpState::TRIPPED_MAX_RUNTIME) {
        Serial.println("[PUMP_CONTROLLER] REJECTED: Pump currently locked in thermal cooldown.");
        return false;
    }

    if (_state != PumpState::RUNNING) {
        setRelay(true);
        _state = PumpState::RUNNING;
        _startTimeMs = millis();
        Serial.println("[PUMP_CONTROLLER] PUMP ENGAGED: Relay coil energized.");
    }
    return true;
}

void PumpController::stop() {
    if (_state == PumpState::RUNNING) {
        setRelay(false);
        _state = PumpState::OFF;
        _stopTimeMs = millis();
        Serial.println("[PUMP_CONTROLLER] PUMP STOPPED: Normal shutdown.");
    }
}

void PumpController::emergencyStop() {
    setRelay(false);
    _state = PumpState::OFF;
    _stopTimeMs = millis();
    Serial.println("[PUMP_CONTROLLER] EMERGENCY STOP: Hard electrical cutoff executed!");
}

uint32_t PumpController::getCurrentRuntimeMs() const {
    if (_state == PumpState::RUNNING) {
        return millis() - _startTimeMs;
    }
    return 0;
}

void PumpController::setRelay(bool energize) {
    uint8_t level = energize ? Config::RELAY_ACTIVE_LOGIC : Config::RELAY_INACTIVE_LOGIC;
    digitalWrite(Pins::RELAY_PUMP, level);
}
