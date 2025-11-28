#include "mytime.h"
#include "driver/i2c.h"


#define I2C_PORT        I2C_NUM_0
#define PCF8563_ADDR    0x51
#define SDA_PIN         21
#define SCL_PIN         22



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
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000
    };


    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, 0, 0, 0);


    return ESP_OK;
}



esp_err_t mytime_get(struct tm *out_time)
{
    if (!out_time) return ESP_ERR_INVALID_ARG;


    uint8_t raw[7];


    i2c_cmd_handle_t cmd = i2c_cmd_link_create();


    
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PCF8563_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x02, true);


    
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PCF8563_ADDR << 1) | I2C_MASTER_READ, true);


    i2c_master_read(cmd, raw, 6, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &raw[6], I2C_MASTER_NACK);


    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);


    
    uint8_t sec   = bcd2bin(raw[0] & 0x7F);
    uint8_t min   = bcd2bin(raw[1] & 0x7F);
    uint8_t hour  = bcd2bin(raw[2] & 0x3F);
    uint8_t day   = bcd2bin(raw[3] & 0x3F);
    uint8_t wday  = bcd2bin(raw[4] & 0x07);
    uint8_t month = bcd2bin(raw[5] & 0x1F);
    uint8_t year  = bcd2bin(raw[6]);


    struct tm t = {0};
    t.tm_sec  = sec;
    t.tm_min  = min;
    t.tm_hour = hour;
    t.tm_mday = day;
    t.tm_wday = wday;
    t.tm_mon  = month - 1;         
    t.tm_year = year + 100;        


    *out_time = t;
    return ESP_OK;
}



esp_err_t mytime_set(const struct tm *in_time)
{
    if (!in_time) return ESP_ERR_INVALID_ARG;


    uint8_t raw[7];


    raw[0] = bin2bcd(in_time->tm_sec)  & 0x7F;
    raw[1] = bin2bcd(in_time->tm_min)  & 0x7F;
    raw[2] = bin2bcd(in_time->tm_hour) & 0x3F;
    raw[3] = bin2bcd(in_time->tm_mday) & 0x3F;
    raw[4] = bin2bcd(in_time->tm_wday) & 0x07;
    raw[5] = bin2bcd(in_time->tm_mon + 1) & 0x1F;
    raw[6] = bin2bcd(in_time->tm_year - 100);


    i2c_cmd_handle_t cmd = i2c_cmd_link_create();


    
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PCF8563_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x02, true);


    
    i2c_master_write(cmd, raw, 7, true);


    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);


    return ESP_OK;
}
