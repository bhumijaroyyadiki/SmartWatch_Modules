#include "power.h"
#include <Wire.h>
#include <AXP20X.h>
#include <Arduino.h>

AXP20X_Class axp;

bool initPower()
{
    
    int ret = axp.begin(Wire, AXP202_SLAVE_ADDRESS);
    Serial.print("AXP begin returned: ");
    Serial.println(ret);

    if (ret != AXP_PASS) {
        Serial.println("AXP INIT FAILED");
        return false;
    }

    axp.setPowerOutPut(AXP202_LDO2, AXP202_ON);
    axp.setPowerOutPut(AXP202_DCDC3, AXP202_ON);
    axp.setPowerOutPut(AXP202_LDO3, AXP202_ON);
    axp.setLDO3Voltage(3300);
    delay(100);

    axp.clearIRQ();

    return true;
}

void enableDisplayPower()
{
    axp.setPowerOutPut(AXP202_LDO2, AXP202_ON);
}

void disableDisplayPower()
{
    axp.setPowerOutPut(AXP202_LDO2, AXP202_OFF);
}

void initPowerButtonIRQ()
{
    uint64_t mask = 0;
    mask |= AXP202_PEK_FALLING_EDGE_IRQ;   // falling edge
    mask |= AXP202_PEK_RISING_EDGE_IRQ;    // rising edge
    mask |= AXP202_PEK_SHORTPRESS_IRQ;     // short press
    mask |= AXP202_PEK_LONGPRESS_IRQ;      // long press

    axp.enableIRQ(mask, true);
    axp.clearIRQ();
}

bool handleAxpIrqAndCheckShortPress()
{
    int r = axp.readIRQ();
    (void)r; 

    bool shortPress = axp.isPEKShortPressIRQ();

    axp.clearIRQ();

    return shortPress;
}

int getBatteryPercentage()
{
    if(!axp.isBatteryConnect())
    {
        return -1;
    }
    return axp.getBattPercentage();
}
