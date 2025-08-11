#include "WiFiManager.h"

WiFiManager::WiFiManager(const char* ssid, const char* password)
    : _ssid(ssid), _password(password), _port(23), _server(_port) {}

void WiFiManager::begin() {
    WiFi.begin(_ssid, _password);

    Serial.print("\n\nConnecting to WiFi");

    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 60000; // 1 minute

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi");
        _server.begin();
        Serial.println("Telnet server started");
    } else {
        Serial.println("\nFailed to connect to WiFi within 1 minute");
    }
}

bool WiFiManager::available() {
    // Accept new client if none connected
    if (_server.hasClient()) {
        if (!_client || !_client.connected()) {
            _client = _server.available();
        } else {
            // Reject new client if already connected
            _server.available().stop();
        }
    }
    // Return true if client connected and has data
    return _client && _client.connected() && _client.available();
}


String WiFiManager::getCommand() {
    if (!_client) return "";

    // Read until newline or timeout
    String cmd = _client.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();
    return cmd;
}

void WiFiManager::print(const String& command) {
    Serial.print(command);
    if (_client && _client.connected()) {
        _client.print(command);
    }
}

void WiFiManager::println(const String& command) {
    Serial.println(command);
    if (_client && _client.connected()) {
        _client.println(command);
    }
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}