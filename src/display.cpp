#include "display.h"
#include "power.h"
#include <Arduino.h>
#include "myWifi.h"
#include "screen.h"
#include "stopwatch.h"

TFT_eSPI tft;
extern MyWiFi wifi;

SemaphoreHandle_t displayMutex = NULL;



void initDisplay()
{
    enableDisplayPower();
    delay(20);

    tft.init();
    tft.setRotation(1);   
    tft.fillScreen(TFT_BLACK);
}



static void drawBackIcon()
{
    tft.drawString("<-", 5, 5, 2);
}



void showHelloWorld()
{
    xSemaphoreTake(displayMutex, portMAX_DELAY);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 30);
    tft.println("Hello, World!");

    drawBackIcon();

    xSemaphoreGive(displayMutex);
}



void showTimeAndDate()
{
    struct tm now;

    if (mytime_get(&now) != ESP_OK)
        return;

    char timeStr[6];
    char dateStr[16];

    sprintf(timeStr, "%02d:%02d", now.tm_hour, now.tm_min);
    sprintf(dateStr, "%02d-%02d-%04d",
            now.tm_mday,
            now.tm_mon + 1,
            now.tm_year + 1900);

    xSemaphoreTake(displayMutex, portMAX_DELAY);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.setTextSize(4);
    tft.drawString(timeStr, 20, 40);

    tft.setTextSize(2);
    tft.drawString(dateStr, 20, 100);

    drawBackIcon();

    xSemaphoreGive(displayMutex);
}



void ShowSteps(uint32_t steps)
{
    xSemaphoreTake(displayMutex, portMAX_DELAY);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.setTextSize(2);
    tft.drawString("Steps", 20, 30);

    tft.setTextSize(4);
    tft.drawNumber(steps, 20, 80);

    drawBackIcon();

    xSemaphoreGive(displayMutex);
}



void showWiFiScreen()
{
    xSemaphoreTake(displayMutex, portMAX_DELAY);

    tft.fillScreen(TFT_BLACK);

    MyWiFiState st = wifi.getState();

    if (st == WIFI_CONNECTED)
    {
        tft.drawString("Wi-Fi Connected", 10, 30, 2);
        tft.drawString("SSID:", 10, 60, 2);
        tft.drawString(wifi.getSSID(), 70, 60, 2);
    }
    else if (st == WIFI_CONNECTING)
    {
        tft.drawString("Wi-Fi Connecting...", 10, 30, 2);
    }
    else
    {
        tft.drawString("Wi-Fi Not Connected", 10, 30, 2);
    }

   
    tft.drawString("[↻]", 200, 10, 2);

    drawBackIcon();

    xSemaphoreGive(displayMutex);
}



void showStopwatchScreen()
{
    xSemaphoreTake(displayMutex, portMAX_DELAY);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.setTextSize(2);
    tft.drawString("Stopwatch", 20, 20);

    uint32_t sec = stopwatch_get_elapsed_seconds();

    uint32_t h = sec / 3600;
    uint32_t m = (sec % 3600) / 60;
    uint32_t s = sec % 60;

    char buf[16];
    snprintf(buf, sizeof(buf), "%02u:%02u:%02u", h, m, s);

    tft.setTextSize(4);
    tft.drawString(buf, 20, 70);

    tft.setTextSize(2);
    tft.drawString("START  STOP  RESET", 20, 150);

    drawBackIcon();

    xSemaphoreGive(displayMutex);
}



void drawBatterySymbol(int x, int y, int percent)
{
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    xSemaphoreTake(displayMutex, portMAX_DELAY);

    int bodyW = 30;
    int bodyH = 14;

    tft.drawRect(x, y, bodyW, bodyH, TFT_WHITE);

   
    tft.fillRect(x + bodyW, y + 4, 3, 6, TFT_WHITE);

    int innerW = bodyW - 4;
    int fillW  = (innerW * percent) / 100;

    tft.fillRect(x + 2, y + 2, fillW, bodyH - 4, TFT_WHITE);

    xSemaphoreGive(displayMutex);
}
