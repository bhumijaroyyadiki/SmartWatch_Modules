#include "power.h"
#include <Wire.h>
#include <AXP20X.h>
#include <Arduino.h>

AXP20X_Class axp;

bool initPower()
{
    
    
    Wire.begin(21, 22);

    int ret = axp.begin(Wire, AXP202_SLAVE_ADDRESS);
    Serial.print("AXP begin returned: ");
    Serial.println(ret);

    if (ret != AXP_PASS) {
        Serial.println("AXP INIT FAILED");
        return false;
    }

    
    axp.setPowerOutPut(AXP202_LDO2, AXP202_ON);
    delay(20);

    
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
    mask |= AXP202_PEK_FALLING_EDGE_IRQ;   
    mask |= AXP202_PEK_RISING_EDGE_IRQ;    
    mask |= AXP202_PEK_SHORTPRESS_IRQ;     
    mask |= AXP202_PEK_LONGPRESS_IRQ;      

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
