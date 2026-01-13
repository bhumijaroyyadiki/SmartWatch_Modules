#pragma once

#include <TFT_eSPI.h>
#include "mytime.h"
extern uint32_t LastSteps;

extern TFT_eSPI tft;
extern SemaphoreHandle_t displayMutex;

void initDisplay();

void showTimeAndDate();
void ShowSteps(uint32_t LastSteps);
void showWiFiScreen();
void showStopwatchScreen();

void drawBatterySymbol(int x, int y, int percent);
