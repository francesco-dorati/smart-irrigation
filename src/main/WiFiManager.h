#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

class WiFiManager {
public:
    WiFiManager(const char* ssid, const char* password);
    void begin();
    bool available();
    String getCommand();
    void print(const String& command);
    void println(const String& command);
    bool isConnected();

private:
    const char* _ssid;
    const char* _password;
    uint16_t _port;
    WiFiServer _server;
    WiFiClient _client;
};

#endif // WIFI_MANAGER_H
