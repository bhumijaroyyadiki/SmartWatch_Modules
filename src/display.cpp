#include "display.h"
#include "power.h"      
#include <Arduino.h>    

TFT_eSPI tft;          

void initDisplay()
{
    enableDisplayPower();
    delay(20);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
}

void showHelloWorld()
{
    // You can choose background
    tft.fillScreen(TFT_BLACK);

    // Set text color and maybe size
    tft.setTextColor(TFT_WHITE, TFT_BLACK);   // fg, bg
    tft.setTextSize(2);                       // 1 = small, 2 = bigger

    // Position cursor (x, y) in pixels
    tft.setCursor(10, 30);

    // Print your text
    tft.println("Hello, World!");
}

void showTimeAndDate()
{
    struct tm now;

    if (mytime_get(&now) == ESP_OK)
    {
        char timeStr[6];
        char dateStr[11];

        sprintf(timeStr, "%02d:%02d", now.tm_hour, now.tm_min);
        sprintf(dateStr, "%02d-%02d-%04d",
                now.tm_mon + 1,
                now.tm_mday,
                now.tm_year + 1900);

        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextSize(2);

        tft.drawString(timeStr, 20, 40);
        tft.drawString(dateStr, 20, 80);
    }
}
