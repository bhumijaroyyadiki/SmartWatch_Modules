#pragma once

#include <axp20x.h>

extern AXP20X_Class axp;

bool initPower();

void enableDisplayPower();
void disableDisplayPower();

void initPowerButtonIRQ();   
bool handleAxpIrqAndCheckShortPress();

extern int getBatteryPercentage();
