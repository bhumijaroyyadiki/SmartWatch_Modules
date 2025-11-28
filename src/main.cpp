




#include <Arduino.h>
#include <WiFi.h>
#include "power.h"
#include "display.h"
#include "mytime.h"

const char* ssid = "POCO X5 Pro 5G";
const char* password = "br10012006";

#define AXP_IRQ_PIN 35   

volatile bool axpIrqFlag = false;
bool displayOn = true;

void IRAM_ATTR axpIrqHandler()
{
    axpIrqFlag = true;   
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("Booting...");

    
    if (!initPower())
    {
        Serial.println("Power init failed");
        
    }
    else
    {
        Serial.println("Power init success");
    }

    
    mytime_init();

    
    initDisplay();
    showTimeAndDate();

    
    pinMode(AXP_IRQ_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(AXP_IRQ_PIN), axpIrqHandler, FALLING);

    
    initPowerButtonIRQ();

    
    WiFi.begin(ssid, password);

    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 20)
    {
        Serial.print(".");
        delay(500);
        attempt++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("WiFi Connected. Syncing time...");

        configTzTime("IST-5:30", "pool.ntp.org");

        struct tm ntp;
        if (getLocalTime(&ntp))
        {
            mytime_set(&ntp);   
            Serial.println("RTC updated from NTP");
            
            showTimeAndDate();
        }
        else
        {
            Serial.println("Failed to get NTP time");
        }
    }
    else
    {
        Serial.println("WiFi not connected — using RTC time");
    }
    
}

void loop()
{
    if (axpIrqFlag)
    {
        axpIrqFlag = false;

        bool shortPress = handleAxpIrqAndCheckShortPress();

        if (shortPress)
        {
            displayOn = !displayOn;

            if (displayOn)
            {
                enableDisplayPower();
                delay(200);        
                initDisplay();     
                showTimeAndDate();
            }
            else
            {
                disableDisplayPower();
            }
        }
    }

    delay(10);
}
