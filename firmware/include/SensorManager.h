/**
 * @file SensorManager.h
 * @brief Multi-Sensor Acquisition, Digital Signal Filtering, and Microclimate Modeling
 * @project AgriFlow-EdgeAI
 */

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <DHT.h>
#include "PinDefinitions.h"
#include "Config.h"
#include "Calibration.h"

struct SensorTelemetry {
    float soilMoisturePct;     // Calibrated Soil Volumetric Water Content (%)
    float waterLevelPct;       // Reservoir Water Capacity (%)
    float temperatureC;        // Ambient Temperature (°C)
    float humidityPct;         // Ambient Relative Humidity (%)
    float vpdKpa;              // Atmospheric Vapor Pressure Deficit (kPa)
    uint16_t rawSoilAdc;       // Raw 12-bit filtered ADC
    uint16_t rawWaterAdc;      // Raw 12-bit filtered ADC
    bool sensorFault;          // True if any sensor reports out-of-bounds or disconnect
};

class SensorManager {
public:
    explicit SensorManager(Calibration& cal);

    /**
     * @brief Initialize hardware ADC attenuation and DHT interface
     */
    void begin();

    /**
     * @brief Sample all physical transducers, apply digital filtering, and update telemetry
     */
    void update();

    /**
     * @brief Retrieve the latest processed sensor metrics
     */
    const SensorTelemetry& getTelemetry() const { return _data; }

private:
    Calibration& _calibration;
    DHT _dht;
    SensorTelemetry _data;

    // Moving Average Ring Buffers
    uint16_t _soilHistory[Config::MOVING_AVERAGE_WINDOW];
    uint16_t _waterHistory[Config::MOVING_AVERAGE_WINDOW];
    uint8_t  _bufferIndex;
    bool     _bufferFilled;

    void updateMovingAverage(uint16_t rawSoil, uint16_t rawWater);
    uint16_t getFilteredSoilAdc() const;
    uint16_t getFilteredWaterAdc() const;
    float calculateVPD(float tempC, float humidityPct) const;
};

#endif // SENSOR_MANAGER_H
