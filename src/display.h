#pragma once

#include <TFT_eSPI.h>
#include "mytime.h"


extern TFT_eSPI tft;

void initDisplay();

void showTimeAndDate();

