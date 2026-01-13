#include "screen.h"
#include "display.h"
#include "stopwatch.h"
#include "power.h"

void showHomeScreen()
{
    xSemaphoreTake(displayMutex, portMAX_DELAY);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    // Title
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println("HOME");

    // STOPWATCH (top-left)
    tft.drawRect(0, 0, 120, 120, TFT_WHITE);
    tft.setCursor(15, 50);
    tft.println("STOPWATCH");

    // WIFI (top-right)
    tft.drawRect(120, 0, 120, 120, TFT_WHITE);
    tft.setCursor(155, 50);
    tft.println("WIFI");

    // STEPS (bottom-left)
    tft.drawRect(0, 120, 120, 120, TFT_WHITE);
    tft.setCursor(30, 170);
    tft.println("STEPS");

    // TIME (bottom-right)
    tft.drawRect(120, 120, 120, 120, TFT_WHITE);
    tft.setCursor(150, 170);
    tft.println("TIME");

    xSemaphoreGive(displayMutex);
}

void setScreen(ScreenID s)

{

   currentScreen = s;
   switch (currentScreen)

   {
   case SCREEN_TIME:
        showTimeAndDate();
        break;
    case SCREEN_STEPS:
        ShowSteps(LastSteps);
        break;

    case SCREEN_WIFI:
        showWiFiScreen();
        break;  

    case SCREEN_STOPWATCH:
        showStopwatchScreen();
        break;

    case SCREEN_HOME:
        showHomeScreen();
        break;

    default:
        break;

   }

}
