#define PCF8563_ADDR    0x51
#pragma once

#include <time.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t mytime_init(void);
esp_err_t mytime_get(struct tm *out_time);
esp_err_t mytime_set(const struct tm *in_time);

#ifdef __cplusplus
}
#endif
