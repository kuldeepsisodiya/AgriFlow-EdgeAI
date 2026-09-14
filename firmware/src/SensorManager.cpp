/**
 * @file SensorManager.cpp
 * @brief Implementation of Sensor Acquisition and Digital Signal Conditioning
 * @project AgriFlow-EdgeAI
 */

#include "SensorManager.h"
#include <cmath>

SensorManager::SensorManager(Calibration& cal)
    : _calibration(cal),
      _dht(Pins::DHT_DATA, DHT22),
      _bufferIndex(0),
      _bufferFilled(false) {
    memset(&_data, 0, sizeof(SensorTelemetry));
    for (uint8_t i = 0; i < Config::MOVING_AVERAGE_WINDOW; ++i) {
        _soilHistory[i] = 0;
        _waterHistory[i] = 0;
    }
}

void SensorManager::begin() {
    pinMode(Pins::SOIL_MOISTURE_ADC, INPUT);
    pinMode(Pins::WATER_LEVEL_ADC, INPUT);

    // Configure ADC1 attenuation for full 0..3.3V dynamic range (11dB)
    analogSetAttenuation(ADC_11db);
    analogReadResolution(12);

    _dht.begin();
    Serial.println("[SENSOR_MANAGER] ADC1 (GPIO34, GPIO35) and DHT22 Initialized.");
}

void SensorManager::update() {
    // 1. Read Raw ADC Samples
    uint16_t currentRawSoil  = analogRead(Pins::SOIL_MOISTURE_ADC);
    uint16_t currentRawWater = analogRead(Pins::WATER_LEVEL_ADC);

    // 2. Feed into 10-Sample Moving Average Filter
    updateMovingAverage(currentRawSoil, currentRawWater);
    uint16_t filteredSoil  = getFilteredSoilAdc();
    uint16_t filteredWater = getFilteredWaterAdc();

    _data.rawSoilAdc  = filteredSoil;
    _data.rawWaterAdc = filteredWater;

    // 3. Map to Engineering Units (%)
    _data.soilMoisturePct = _calibration.rawToSoilMoisturePct(filteredSoil);
    _data.waterLevelPct   = _calibration.rawToWaterLevelPct(filteredWater);

    // 4. Sample Microclimate DHT22 Sensor
    float t = _dht.readTemperature();
    float h = _dht.readHumidity();

    if (isnan(t) || isnan(h)) {
        _data.sensorFault = true;
        Serial.println("[SENSOR_MANAGER] WARNING: DHT22 Read Timeout or CRC Failure!");
    } else {
        _data.temperatureC = t;
        _data.humidityPct  = h;
        _data.vpdKpa       = calculateVPD(t, h);
        _data.sensorFault  = false;
    }
}

void SensorManager::updateMovingAverage(uint16_t rawSoil, uint16_t rawWater) {
    _soilHistory[_bufferIndex]  = rawSoil;
    _waterHistory[_bufferIndex] = rawWater;

    _bufferIndex = (_bufferIndex + 1) % Config::MOVING_AVERAGE_WINDOW;
    if (_bufferIndex == 0) {
        _bufferFilled = true;
    }
}

uint16_t SensorManager::getFilteredSoilAdc() const {
    uint8_t count = _bufferFilled ? Config::MOVING_AVERAGE_WINDOW : _bufferIndex;
    if (count == 0) return _soilHistory[0];

    uint32_t sum = 0;
    for (uint8_t i = 0; i < count; ++i) {
        sum += _soilHistory[i];
    }
    return static_cast<uint16_t>(sum / count);
}

uint16_t SensorManager::getFilteredWaterAdc() const {
    uint8_t count = _bufferFilled ? Config::MOVING_AVERAGE_WINDOW : _bufferIndex;
    if (count == 0) return _waterHistory[0];

    uint32_t sum = 0;
    for (uint8_t i = 0; i < count; ++i) {
        sum += _waterHistory[i];
    }
    return static_cast<uint16_t>(sum / count);
}

float SensorManager::calculateVPD(float tempC, float humidityPct) const {
    // Tetens equation for saturation vapor pressure Es (kPa)
    float es = 0.61078f * expf((17.27f * tempC) / (tempC + 237.3f));
    // Actual vapor pressure Ea (kPa)
    float ea = es * (humidityPct / 100.0f);
    // Vapor Pressure Deficit (VPD) in kPa
    float vpd = es - ea;
    return fmaxf(0.0f, vpd);
}
