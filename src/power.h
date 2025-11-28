#pragma once

#include <axp20x.h>

extern AXP20X_Class axp;

bool initPower();

void enableDisplayPower();
void disableDisplayPower();

void initPowerButtonIRQ();   // enables PEK short-press interrupt in AXP
bool handleAxpIrqAndCheckShortPress();
