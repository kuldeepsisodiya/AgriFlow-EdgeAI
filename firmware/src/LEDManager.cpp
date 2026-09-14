/**
 * @file LEDManager.cpp
 * @brief Implementation of Visual Telemetry Indicator
 * @project AgriFlow-EdgeAI
 */

#include "LEDManager.h"

LEDManager::LEDManager()
    : _currentPattern(VisualPattern::CONNECTING),
      _lastToggleMs(0),
      _ledState(false) {}

void LEDManager::begin() {
    pinMode(Pins::STATUS_LED, OUTPUT);
    writeLED(false);
}

void LEDManager::setPattern(VisualPattern pattern) {
    if (_currentPattern != pattern) {
        _currentPattern = pattern;
        _lastToggleMs = millis();
        if (pattern == VisualPattern::OFF) {
            writeLED(false);
        } else if (pattern == VisualPattern::SOLID_ON) {
            writeLED(true);
        }
    }
}

void LEDManager::update() {
    uint32_t now = millis();

    switch (_currentPattern) {
        case VisualPattern::CONNECTING:
            if (now - _lastToggleMs >= 250) {
                toggleLED();
                _lastToggleMs = now;
            }
            break;

        case VisualPattern::IDLE_HEALTHY:
            if (now - _lastToggleMs >= 1000) {
                toggleLED();
                _lastToggleMs = now;
            }
            break;

        case VisualPattern::IRRIGATING:
            if (now - _lastToggleMs >= 500) {
                toggleLED();
                _lastToggleMs = now;
            }
            break;

        case VisualPattern::ERROR_ALERT:
            if (now - _lastToggleMs >= 100) {
                toggleLED();
                _lastToggleMs = now;
            }
            break;

        case VisualPattern::SOLID_ON:
        case VisualPattern::OFF:
        default:
            break;
    }
}

void LEDManager::toggleLED() {
    _ledState = !_ledState;
    writeLED(_ledState);
}

void LEDManager::writeLED(bool state) {
    _ledState = state;
    digitalWrite(Pins::STATUS_LED, state ? HIGH : LOW);
}
