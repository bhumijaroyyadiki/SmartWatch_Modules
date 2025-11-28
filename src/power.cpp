#include "power.h"
#include <Wire.h>
#include <AXP20X.h>
#include <Arduino.h>

AXP20X_Class axp;

bool initPower()
{
    // Initialize I2C (Wire) BEFORE axp.begin
    // Use the same SDA/SCL pins used by the rest of the code
    Wire.begin(21, 22);

    int ret = axp.begin(Wire, AXP202_SLAVE_ADDRESS);
    Serial.print("AXP begin returned: ");
    Serial.println(ret);

    if (ret != AXP_PASS) {
        Serial.println("AXP INIT FAILED");
        return false;
    }

    // Ensure LDO2 (display rail) is ON
    axp.setPowerOutPut(AXP202_LDO2, AXP202_ON);
    delay(20);

    // Clear any pending IRQs
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
    // Build mask using the defined IRQ bitmasks (no shifting)
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
    // library's readIRQ() populates internal irq array and returns status code
    int r = axp.readIRQ();
    (void)r; // ignore numeric return (AXP_PASS), use helper to check flags

    // Use the library helper to check for short press
    bool shortPress = axp.isPEKShortPressIRQ();

    // Clear IRQ flags in the chip
    axp.clearIRQ();

    return shortPress;
}
