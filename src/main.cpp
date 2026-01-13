// I2C devices on T-Watch:
// 0x35 - AXP202 PMIC
// 0x19 - BMA423 Accelerometer
// 0x51 - PCF8563 RTC
// GPIO for IRQ : 35
// LDO2 - display power
// I2c for FT6336U capacitive touch: 0x38 - falling edge IrQ on IO38

#include <Arduino.h>
#include <WiFi.h>
#include "power.h"
#include "display.h"
#include "mytime.h"
#include "accel.h"
#include "mywifi.h"
#include "screen.h"
#include "stopwatch.h"

volatile ScreenID currentScreen = SCREEN_TIME;

MyWiFi wifi("POCO X5 Pro 5G", "br10012006");
bool timeSynced = false;

TwoWire TouchWire = TwoWire(1);

const char* ssid = "POCO X5 Pro 5G";
const char* password = "br10012006";

#define AXP_IRQ_PIN 35

volatile bool axpIrqFlag = false;
bool displayOn = true;

volatile bool touchIrqFlag = false;

uint32_t LastReadTime = 0;
uint32_t LastSteps = 0;

//RTOS Task - 4 (1)
void touch_task(void *pv)
{
    while(1)
    {
        if (touchIrqFlag)
        {
            touchIrqFlag = false;

            uint16_t x, y;

            if (!cpt_getTouch(&x, &y))
            continue;

            // raw values
            Serial.print("RAW: ");
            Serial.print(x);
            Serial.print(", ");
            Serial.println(y);

            // rotate touch to match screen
            uint16_t tx = y;
            uint16_t ty = 240 - x;

            Serial.print("MAP: ");
            Serial.print(tx);
            Serial.print(", ");
            Serial.println(ty);

            // BACK
            if (tx > 200 && ty > 200)   
            {
                setScreen(SCREEN_HOME);
                continue;
            }

            // HOME buttons
            if (currentScreen == SCREEN_HOME)
            {
                if (tx < 120 && ty < 120) setScreen(SCREEN_TIME);
                else if (tx >= 120 && ty < 120) setScreen(SCREEN_STEPS);
                else if (tx < 120 && ty >= 120) setScreen(SCREEN_WIFI);
                else if (tx >= 120 && ty >= 120) setScreen(SCREEN_STOPWATCH);
            }


            // ---------- WIFI BACK BUTTON ----------
            if (currentScreen == SCREEN_WIFI)
            {
                if (x < 40 && y < 40)
                {
                    setScreen(SCREEN_TIME);
                    continue;
                }
            }

            // ---------- STOPWATCH BUTTONS ----------
          if (currentScreen == SCREEN_STOPWATCH)
            {
                // START  (left)
                if (tx < 80 && ty > 140)
                {
                    stopwatch_send_command(STOPWATCH_CMD_START);
                    continue;
                }

                // STOP (middle)
                if (tx >= 80 && tx < 160 && ty > 140)
                {
                    stopwatch_send_command(STOPWATCH_CMD_STOP);
                    continue;
                }

                // RESET (right)
                if (tx >= 160 && ty > 140)
                {
                    stopwatch_send_command(STOPWATCH_CMD_RESET);
                    continue;
                }
            }

        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
//rtos - 4(4)- wifi
 // ---------- WIFI SCREEN AUTO REFRESH ----------
 void display_task(void *pv)
{
    static MyWiFiState lastState = WIFI_NOT_CONNECTED;

    while(1)
    {
    if (currentScreen == SCREEN_WIFI)
    {
        MyWiFiState now = wifi.getState();
        if (now != lastState)
        {
            setScreen(SCREEN_WIFI);
            lastState = now;
        }
    }

    // ---------- STOPWATCH LIVE REFRESH ----------
    if (currentScreen == SCREEN_STOPWATCH)
    {
        showStopwatchScreen();
    }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

//rtos -4(5)
void network_task(void *pv)
{
    static MyWiFiState lastState = WIFI_NOT_CONNECTED;

    while(1)
    {
        // AUTO TIME SYNC
        if (!timeSynced && wifi.getState() == WIFI_CONNECTED)
        {
            struct tm t;
            if (getLocalTime(&t))
            {
                mytime_set(&t);
                timeSynced = true;
            }
        }

        // detect WiFi state change
        MyWiFiState now = wifi.getState();
        if (now != lastState)
        {
            
            lastState = now;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));  
    }
}

// -------------------- IRQ HANDLERS -------------------

void IRAM_ATTR axpIrqHandler()
{
    axpIrqFlag = true;
}

void IRAM_ATTR touchIrqHandler()
{
    touchIrqFlag = true;
}

// -------------------- TOUCH DRIVER UTILITY -------------------

bool cpt_init()
{
    return true;
}

uint8_t ft_readReg(uint8_t reg)
{
    TouchWire.beginTransmission(FT6336U_ADDR);
    TouchWire.write(reg);
    TouchWire.endTransmission(false);

    TouchWire.requestFrom(FT6336U_ADDR, 1);
    if (TouchWire.available())
        return TouchWire.read();

    return 0;
}

bool cpt_getTouch(uint16_t *x, uint16_t *y)
{
    uint8_t touches = ft_readReg(0x02);

    if ((touches & 0x0F) == 0)
        return false;

    uint16_t xh = ft_readReg(0x03);
    uint16_t xl = ft_readReg(0x04);
    uint16_t yh = ft_readReg(0x05);
    uint16_t yl = ft_readReg(0x06);

    *x = ((xh & 0x0F) << 8) | xl;
    *y = ((yh & 0x0F) << 8) | yl;

    return true;
}
//rtos -4(6)
void step_task(void *pv)
{
    while (1)
    {
        uint32_t steps = ACC.getPedometerCounter();

        if (steps != LastSteps)
        {
            LastSteps = steps;

            
            if (currentScreen == SCREEN_STEPS)
                ShowSteps(LastSteps);
        }

        vTaskDelay(pdMS_TO_TICKS(500));   
    }
}

// -------------------- SETUP -------------------

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Wire.begin(21,22);
    Wire.setClock(400000);

    TouchWire.begin(23, 32);

    mytime_init();
    cpt_init();
    
    displayMutex = xSemaphoreCreateMutex();    
    
    // ---- STOPWATCH INIT ----
    QueueHandle_t swQueue = xQueueCreate(5, sizeof(StopwatchCommand_t));
    stopwatch_init(swQueue);

    initDisplay();

    wifi.init();
    wifi.connect();
    

    currentScreen = SCREEN_HOME;
    setScreen(SCREEN_HOME);

    pinMode(38, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(38), touchIrqHandler, FALLING);

    xTaskCreate(touch_task, "touch", 4096, NULL, 3, NULL);
    xTaskCreate(step_task, "step", 4096, NULL, 2, NULL);
    xTaskCreate(display_task, "display", 4096, NULL, 2, NULL);
    xTaskCreate(network_task, "network", 4096, NULL, 1, NULL);

    if (!initPower())
    {
        Serial.println("Power init failed");
    }
    else
    {
        Serial.println("Power init success");

        InitAcc();
        SensorInit();

        pinMode(AXP_IRQ_PIN, INPUT);
        attachInterrupt(digitalPinToInterrupt(AXP_IRQ_PIN), axpIrqHandler, FALLING);
    }

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
        configTzTime("IST-5:30", "pool.ntp.org");

        struct tm ntp;
        if (getLocalTime(&ntp))
        {
            mytime_set(&ntp);
            setScreen(SCREEN_HOME);
        }
    }
}

// -------------------- LOOP -------------------

void loop()
{
    // ---------- POWER BUTTON ----------
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
                setScreen(currentScreen);
            }
            else
            {
                disableDisplayPower();
            }
        }
    }

    delay(10);
}
