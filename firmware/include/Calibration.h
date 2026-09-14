/**
 * @file Calibration.h
 * @brief Non-Volatile Storage (NVS) Calibration Manager for Transducers
 * @project AgriFlow-EdgeAI
 */

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>
#include <Preferences.h>
#include "Config.h"

class Calibration {
public:
    Calibration();

    /**
     * @brief Initialize NVS storage and load calibration profiles
     */
    void begin();

    /**
     * @brief Map raw 12-bit ADC value to calibrated Volumetric Water Content percentage (0..100%)
     */
    float rawToSoilMoisturePct(uint16_t rawAdc) const;

    /**
     * @brief Map raw 12-bit ADC value to reservoir water level percentage (0..100%)
     */
    float rawToWaterLevelPct(uint16_t rawAdc) const;

    /**
     * @brief Update dry air calibration point in NVS
     */
    void setSoilDryAdc(uint16_t adcVal);

    /**
     * @brief Update water saturated calibration point in NVS
     */
    void setSoilWetAdc(uint16_t adcVal);

    uint16_t getSoilDryAdc() const { return _soilDryAdc; }
    uint16_t getSoilWetAdc() const { return _soilWetAdc; }

private:
    Preferences _prefs;
    uint16_t _soilDryAdc;
    uint16_t _soilWetAdc;
    uint16_t _waterEmptyAdc;
    uint16_t _waterFullAdc;
};

#endif // CALIBRATION_H
