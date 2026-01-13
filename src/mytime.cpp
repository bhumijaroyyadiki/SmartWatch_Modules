#include "mytime.h"
#include <Wire.h>


#define PCF8563_ADDR 0x51
#define FT6336U_ADDR 0x38
#define SDA_PIN      21
#define SCL_PIN      22


static uint8_t bcd2bin(uint8_t v)
{
    return ((v >> 4) * 10) + (v & 0x0F);
}

static uint8_t bin2bcd(uint8_t v)
{
    return ((v / 10) << 4) | (v % 10);
}

esp_err_t mytime_init(void)
{  
    return ESP_OK;
}

esp_err_t mytime_get(struct tm *out_time)
{
    if (!out_time) return ESP_ERR_INVALID_ARG;

    Wire.beginTransmission(PCF8563_ADDR);
    Wire.write(0x02);
    Wire.endTransmission(false);

    Wire.requestFrom(PCF8563_ADDR, 7);

    uint8_t raw[7];
    for (int i = 0; i < 7; i++) raw[i] = Wire.read();

    struct tm t = {0};
    t.tm_sec  = bcd2bin(raw[0] & 0x7F);
    t.tm_min  = bcd2bin(raw[1] & 0x7F);
    t.tm_hour = bcd2bin(raw[2] & 0x3F);
    t.tm_mday = bcd2bin(raw[3] & 0x3F);
    t.tm_wday = bcd2bin(raw[4] & 0x07);
    t.tm_mon  = bcd2bin(raw[5] & 0x1F) - 1;
    t.tm_year = bcd2bin(raw[6]) + 100;

    *out_time = t;
    return ESP_OK;
}

esp_err_t mytime_set(const struct tm *in_time)
{
    if (!in_time) return ESP_ERR_INVALID_ARG;

    Wire.beginTransmission(PCF8563_ADDR);
    Wire.write(0x02);

    Wire.write(bin2bcd(in_time->tm_sec)  & 0x7F);
    Wire.write(bin2bcd(in_time->tm_min)  & 0x7F);
    Wire.write(bin2bcd(in_time->tm_hour) & 0x3F);
    Wire.write(bin2bcd(in_time->tm_mday) & 0x3F);
    Wire.write(bin2bcd(in_time->tm_wday) & 0x07);
    Wire.write(bin2bcd(in_time->tm_mon + 1) & 0x1F);
    Wire.write(bin2bcd(in_time->tm_year - 100));

    Wire.endTransmission();
    return ESP_OK;
}
