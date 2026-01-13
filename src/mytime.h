#pragma once

#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <Arduino.h>    


#ifdef __cplusplus
extern "C" {
#endif

#define PCF8563_ADDR  0x51
#define FT6336U_ADDR  0x38

// RTC APIs
esp_err_t mytime_init(void);
esp_err_t mytime_get(struct tm *out_time);
esp_err_t mytime_set(const struct tm *in_time);

// Touch APIs
uint8_t ft_readReg(uint8_t reg);
void ft_writeReg(uint8_t reg, uint8_t data); 
bool cpt_init(void);
bool cpt_getTouch(uint16_t *x, uint16_t *y);

#ifdef __cplusplus
}
#endif
