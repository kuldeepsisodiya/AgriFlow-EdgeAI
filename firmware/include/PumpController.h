/**
 * @file PumpController.h
 * @brief Actuator Driver, Inductive Safety Interlocks, and Watchdog Protection
 * @project AgriFlow-EdgeAI
 */

#ifndef PUMP_CONTROLLER_H
#define PUMP_CONTROLLER_H

#include <Arduino.h>
#include "PinDefinitions.h"
#include "Config.h"

enum class PumpState {
    OFF,
    RUNNING,
    TRIPPED_MAX_RUNTIME,
    TRIPPED_DRY_RUN,
    IN_COOLDOWN
};

class PumpController {
public:
    PumpController();

    /**
     * @brief Initialize relay GPIO with fail-safe HIGH default
     */
    void begin();

    /**
     * @brief Periodic safety watchdog checking runtime limits and cooldown
     */
    void update();

    /**
     * @brief Request the pump to engage (subject to safety interlocks)
     * @param reservoirLevelPct Current water reservoir percentage
     * @return True if pump was successfully engaged, false if interlocked
     */
    bool start(float reservoirLevelPct);

    /**
     * @brief Immediately disengage the pump and open relay contacts
     */
    void stop();

    /**
     * @brief Emergency hard kill switch (bypasses logic)
     */
    void emergencyStop();

    bool isRunning() const { return _state == PumpState::RUNNING; }
    PumpState getState() const { return _state; }
    uint32_t getCurrentRuntimeMs() const;

private:
    PumpState _state;
    uint32_t  _startTimeMs;
    uint32_t  _stopTimeMs;

    void setRelay(bool energize);
};

#endif // PUMP_CONTROLLER_H
