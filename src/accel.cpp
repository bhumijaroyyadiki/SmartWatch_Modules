#include "accel.h"
#include <Arduino.h>
SensorBMA423 ACC;

bool InitAcc()
{
    
    if (!ACC.init(Wire, 21, 22, BMA_I2C_ADDRESS))
    {
        Serial.println("BMA423 init failed");
        return false;
    }

    Serial.println("BMA423 init OK");
    ACC.enableAccelerometer();
    ACC.configAccelerometer();
    Serial.println("Accelerometer enabled");

    ACC.enableFeature(SensorBMA423::FEATURE_STEP_CNTR, 1);
    ACC.enablePedometer(true);
    delay(20);

    return true;

}

bool SensorInit()
{
    pinMode(BMA_GPIO_INT_PIN, INPUT);
    return true;
    
}