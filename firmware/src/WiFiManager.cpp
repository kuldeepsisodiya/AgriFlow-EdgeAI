/**
 * @file WiFiManager.cpp
 * @brief Implementation of Non-Blocking Wi-Fi Connection Manager
 * @project AgriFlow-EdgeAI
 */

#include "WiFiManager.h"

WiFiManager::WiFiManager()
    : _lastReconnectAttemptMs(0),
      _wasConnected(false) {}

void WiFiManager::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false); // We control reconnection to prevent thread-blocking
    connectAsync();
}

void WiFiManager::connectAsync() {
    Serial.printf("[WIFI_MANAGER] Initiating connection to SSID: '%s'...\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    _lastReconnectAttemptMs = millis();
}

void WiFiManager::update() {
    bool currentlyConnected = (WiFi.status() == WL_CONNECTED);

    if (currentlyConnected && !_wasConnected) {
        _wasConnected = true;
        Serial.printf("[WIFI_MANAGER] CONNECTED! Local IP: %s, RSSI: %d dBm\n",
                      WiFi.localIP().toString().c_str(), WiFi.RSSI());
    } else if (!currentlyConnected && _wasConnected) {
        _wasConnected = false;
        Serial.println("[WIFI_MANAGER] WARNING: Wi-Fi Link Lost! Operating in Offline Autonomous Edge Mode.");
        _lastReconnectAttemptMs = millis();
    } else if (!currentlyConnected) {
        // Non-blocking periodic reconnection attempt
        uint32_t now = millis();
        if (now - _lastReconnectAttemptMs >= Config::WIFI_RECONNECT_BACKOFF_MS) {
            _lastReconnectAttemptMs = now;
            Serial.println("[WIFI_MANAGER] Attempting background re-association...");
            WiFi.disconnect();
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        }
    }
}

bool WiFiManager::isConnected() const {
    return (WiFi.status() == WL_CONNECTED);
}

int8_t WiFiManager::getRSSI() const {
    return WiFi.RSSI();
}

String WiFiManager::getIPAddress() const {
    return WiFi.localIP().toString();
}
