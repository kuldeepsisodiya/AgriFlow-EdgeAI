/**
 * @file Calibration.cpp
 * @brief Implementation of NVS Calibration Profile Manager
 * @project AgriFlow-EdgeAI
 */

#include "Calibration.h"

Calibration::Calibration()
    : _soilDryAdc(Config::SOIL_ADC_DRY_AIR),
      _soilWetAdc(Config::SOIL_ADC_SATURATED),
      _waterEmptyAdc(Config::WATER_ADC_EMPTY),
      _waterFullAdc(Config::WATER_ADC_FULL) {}

void Calibration::begin() {
    _prefs.begin("agri_cal", false);
    _soilDryAdc   = _prefs.getUShort("soil_dry", Config::SOIL_ADC_DRY_AIR);
    _soilWetAdc   = _prefs.getUShort("soil_wet", Config::SOIL_ADC_SATURATED);
    _waterEmptyAdc = _prefs.getUShort("wat_empty", Config::WATER_ADC_EMPTY);
    _waterFullAdc  = _prefs.getUShort("wat_full", Config::WATER_ADC_FULL);
    _prefs.end();

    Serial.printf("[CALIBRATION] Loaded NVS Profile -> SoilDry: %u, SoilWet: %u, WaterEmpty: %u, WaterFull: %u\n",
                  _soilDryAdc, _soilWetAdc, _waterEmptyAdc, _waterFullAdc);
}

float Calibration::rawToSoilMoisturePct(uint16_t rawAdc) const {
    // Capacitive sensor outputs higher voltage in dry air and lower in water
    if (_soilDryAdc <= _soilWetAdc) {
        return 0.0f; // Prevent division by zero
    }

    if (rawAdc >= _soilDryAdc) return 0.0f;
    if (rawAdc <= _soilWetAdc) return 100.0f;

    float pct = (float)(_soilDryAdc - rawAdc) / (float)(_soilDryAdc - _soilWetAdc) * 100.0f;
    return constrain(pct, 0.0f, 100.0f);
}

float Calibration::rawToWaterLevelPct(uint16_t rawAdc) const {
    if (_waterFullAdc <= _waterEmptyAdc) {
        return 0.0f;
    }

    if (rawAdc <= _waterEmptyAdc) return 0.0f;
    if (rawAdc >= _waterFullAdc)  return 100.0f;

    float pct = (float)(rawAdc - _waterEmptyAdc) / (float)(_waterFullAdc - _waterEmptyAdc) * 100.0f;
    return constrain(pct, 0.0f, 100.0f);
}

void Calibration::setSoilDryAdc(uint16_t adcVal) {
    _soilDryAdc = adcVal;
    _prefs.begin("agri_cal", false);
    _prefs.putUShort("soil_dry", _soilDryAdc);
    _prefs.end();
    Serial.printf("[CALIBRATION] Saved new Soil Dry ADC: %u\n", _soilDryAdc);
}

void Calibration::setSoilWetAdc(uint16_t adcVal) {
    _soilWetAdc = adcVal;
    _prefs.begin("agri_cal", false);
    _prefs.putUShort("soil_wet", _soilWetAdc);
    _prefs.end();
    Serial.printf("[CALIBRATION] Saved new Soil Wet ADC: %u\n", _soilWetAdc);
}
