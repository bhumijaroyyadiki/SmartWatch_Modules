#pragma once
#include <WiFi.h>

enum MyWiFiState {
    WIFI_NOT_CONNECTED,
    WIFI_CONNECTING,
    WIFI_CONNECTED
};

class MyWiFi {
public:
    bool isConnecting();
    MyWiFi(const char* ssid, const char* password);

    void init();
    void connect();
    MyWiFiState getState();
    String getSSID();

private:
    static void WiFiEventHandler(WiFiEvent_t event, WiFiEventInfo_t info);
    void handleEvent(WiFiEvent_t event);

    const char* _ssid;
    const char* _password;

    MyWiFiState _state;
    String _currentSSID;
    unsigned long _lastConnectAttempt;
};
