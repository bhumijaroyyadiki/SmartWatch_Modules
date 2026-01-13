#include "mywifi.h"

extern MyWiFi wifi;

MyWiFi::MyWiFi(const char* ssid, const char* password)
: _ssid(ssid), _password(password)
{
    _state = WIFI_NOT_CONNECTED;
    _lastConnectAttempt = 0;
}

void MyWiFi::init() {
    WiFi.mode(WIFI_STA);
    WiFi.onEvent(WiFiEventHandler);  
}

void MyWiFi::connect() {
    unsigned long now = millis();
    if (now - _lastConnectAttempt < 3000) return;

    _lastConnectAttempt = now;
    _state = WIFI_CONNECTING;

    WiFi.begin(_ssid, _password);
}

void MyWiFi::WiFiEventHandler(WiFiEvent_t event, WiFiEventInfo_t info) {
    wifi.handleEvent(event);  
}

void MyWiFi::handleEvent(WiFiEvent_t event) {

    if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP) {
        _state = WIFI_CONNECTED;
        _currentSSID = WiFi.SSID();
    }

    else if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
        _state = WIFI_NOT_CONNECTED;
        _currentSSID = "";
    }
}

MyWiFiState MyWiFi::getState() {
    return _state;
}

String MyWiFi::getSSID() {
    return _currentSSID;
}

bool MyWiFi::isConnecting() {
    return _state == WIFI_CONNECTING;
}
