#define BMA_GPIO_INT_PIN 39
#define BMA_I2C_ADDRESS 0x19
#include <SensorBMA423.hpp>

extern SensorBMA423 ACC;
bool InitAcc();
bool SensorInit();