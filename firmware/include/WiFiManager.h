/**
 * @file WiFiManager.h
 * @brief Non-Blocking Wi-Fi Connection Manager & Offline Edge Fallback
 * @project AgriFlow-EdgeAI
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "Config.h"

class WiFiManager {
public:
    WiFiManager();

    /**
     * @brief Configure station mode and initiate connection asynchronously
     */
    void begin();

    /**
     * @brief Periodic non-blocking link monitor and auto-reconnection handler
     */
    void update();

    bool isConnected() const;
    int8_t getRSSI() const;
    String getIPAddress() const;

private:
    uint32_t _lastReconnectAttemptMs;
    bool     _wasConnected;

    void connectAsync();
};

#endif // WIFI_MANAGER_H
