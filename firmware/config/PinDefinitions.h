/**
 * @file PinDefinitions.h
 * @brief Hardware GPIO Pin Allocations with Electrical Engineering Justifications
 * @project AgriFlow-EdgeAI
 *
 * All pins are assigned to avoid ESP32 boot strapping conflicts, Wi-Fi SAR ADC conflicts,
 * and high-voltage inductive interference.
 */

#ifndef PIN_DEFINITIONS_H
#define PIN_DEFINITIONS_H

#include <Arduino.h>

namespace Pins {

    /**
     * @brief Capacitive Soil Moisture Sensor Analog Input
     * @pin GPIO35 (ADC1_CH7)
     * @rationale ADC1 is strictly separated from the Wi-Fi subsystem. On the ESP32,
     *            ADC2 pins cannot be used while the Wi-Fi transceiver is transmitting.
     *            GPIO35 is an input-only pin without internal pullups, perfectly suited for
     *            an external analog transducer.
     */
    constexpr uint8_t SOIL_MOISTURE_ADC = 35;

    /**
     * @brief Water Reservoir Depth / Level Sensor Analog Input
     * @pin GPIO34 (ADC1_CH6)
     * @rationale ADC1 channel allows continuous level telemetry concurrently with active
     *            TLS Wi-Fi streaming. Input-only pad protects against accidental drive states.
     */
    constexpr uint8_t WATER_LEVEL_ADC = 34;

    /**
     * @brief DHT22 (AM2302) Digital Temperature & Humidity Bus
     * @pin GPIO4
     * @rationale General-purpose bi-directional GPIO supporting open-drain single-bus
     *            timing protocols. Not involved in boot strapping sequence. Requires
     *            external or strong internal 10kΩ pull-up resistor to 3.3V.
     */
    constexpr uint8_t DHT_DATA = 4;

    /**
     * @brief Optocoupled Relay Control Output for 12V Water Pump
     * @pin GPIO5
     * @rationale ESP32 strapping pin that outputs HIGH at boot and stabilizes within
     *            microseconds. When using an Active-LOW relay module, this guarantees
     *            the pump remains safely DE-ENERGIZED during microcontroller reboot or crash.
     */
    constexpr uint8_t RELAY_PUMP = 5;

    /**
     * @brief Status LED Indicator (RGB / Single Status Channel)
     * @pin GPIO21
     * @rationale Default I2C SDA pin re-assigned as high-speed digital output to provide
     *            visual heartbeat, error blinking, and connection feedback.
     */
    constexpr uint8_t STATUS_LED = 21;

} // namespace Pins

#endif // PIN_DEFINITIONS_H
