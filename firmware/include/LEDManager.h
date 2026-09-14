/**
 * @file LEDManager.h
 * @brief Non-Blocking Visual Telemetry Status Indicator
 * @project AgriFlow-EdgeAI
 */

#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <Arduino.h>
#include "PinDefinitions.h"

enum class VisualPattern {
    OFF,
    SOLID_ON,
    CONNECTING,      // Rapid 250ms cadence
    IDLE_HEALTHY,    // Slow 1000ms breathing pulse
    IRRIGATING,      // Synchronized 500ms blink
    ERROR_ALERT      // Rapid 100ms emergency strobe
};

class LEDManager {
public:
    LEDManager();

    void begin();
    void update();
    void setPattern(VisualPattern pattern);

private:
    VisualPattern _currentPattern;
    uint32_t      _lastToggleMs;
    bool          _ledState;

    void toggleLED();
    void writeLED(bool state);
};

#endif // LED_MANAGER_H
