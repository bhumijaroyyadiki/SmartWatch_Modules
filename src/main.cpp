// I2C devices on T-Watch:
// 0x35 - AXP202 PMIC
// 0x19 - BMA423 Accelerometer
// 0x51 - PCF8563 RTC

#include <Arduino.h>
#include <WiFi.h>
#include "power.h"
#include "display.h"
#include "mytime.h"

const char* ssid = "POCO X5 Pro 5G";
const char* password = "br10012006";

#define AXP_IRQ_PIN 35   // correct for T-Watch 2020 V3

volatile bool axpIrqFlag = false;
bool displayOn = true;

void IRAM_ATTR axpIrqHandler()
{
    axpIrqFlag = true;   // ISR: keep very small
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("Booting...");

    // Init power (AXP202) - this also calls Wire.begin
    if (!initPower())
    {
        Serial.println("Power init failed");
        // still continue so you can debug
    }
    else
    {
        Serial.println("Power init success");
    }

    // Init RTC I2C wrapper (uses Wire, but doesn't re-install driver)
    mytime_init();

    // Initialize display (power already enabled in initPower)
    initDisplay();
    showTimeAndDate();

    // Configure AXP IRQ pin and attach ISR for side button (PEK)
    pinMode(AXP_IRQ_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(AXP_IRQ_PIN), axpIrqHandler, FALLING);

    // Enable AXP PEK short-press IRQ in the PMIC
    initPowerButtonIRQ();

    // ----------- WiFi + NTP Sync (optional) -----------------
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
            mytime_set(&ntp);   // Store time into RTC
            Serial.println("RTC updated from NTP");
            // update displayed time
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
    // --------------------------------------------------------
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
                delay(200);        // allow power rails to stabilise
                initDisplay();     // re-init the TFT after powering on
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
