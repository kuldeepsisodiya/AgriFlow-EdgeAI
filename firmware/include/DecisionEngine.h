/**
 * @file DecisionEngine.h
 * @brief Core Edge Intelligence, Hysteretic State Machine & Failsafe Engine
 * @project AgriFlow-EdgeAI
 */

#ifndef DECISION_ENGINE_H
#define DECISION_ENGINE_H

#include <Arduino.h>
#include "Config.h"
#include "SensorManager.h"
#include "PumpController.h"
#include "LEDManager.h"

enum class OperationMode {
    AUTOMATIC = 0,
    MANUAL    = 1
};

enum class SystemState {
    BOOT_INIT,
    MONITORING_IDLE,
    AUTO_WATERING,
    MANUAL_OVERRIDE,
    TANK_EMPTY_LOCKED,
    SENSOR_FAULT_FAILSAFE
};

class DecisionEngine {
public:
    DecisionEngine(SensorManager& sensors, PumpController& pump, LEDManager& led);

    void begin();

    /**
     * @brief Periodic evaluation of sensor data, safety criteria, and actuation
     */
    void evaluate();

    /**
     * @brief Switch operating mode between AUTOMATIC and MANUAL
     */
    void setMode(OperationMode mode);

    /**
     * @brief Manual pump toggle command received from Blynk / physical button
     */
    void setManualPumpCommand(bool engage);

    OperationMode getMode() const { return _mode; }
    SystemState getState() const { return _state; }
    const char* getStateString() const;

private:
    SensorManager&  _sensors;
    PumpController& _pump;
    LEDManager&     _led;

    OperationMode   _mode;
    SystemState     _state;
    bool            _manualPumpRequest;
    uint32_t        _manualStartMs;

    void handleAutomaticMode(const SensorTelemetry& telemetry);
    void handleManualMode(const SensorTelemetry& telemetry);
    void updateVisualIndicator();
};

#endif // DECISION_ENGINE_H
