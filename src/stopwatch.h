#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include <stdint.h>
typedef enum {
    STOPWATCH_CMD_START,
    STOPWATCH_CMD_STOP,
    STOPWATCH_CMD_RESET,
    
} StopwatchCommand_t;

extern uint32_t sec;
extern uint32_t hours;
extern uint32_t minutes;
extern uint32_t seconds;

//void stopwatch_init(void *cmd_queue);
void stopwatch_init(QueueHandle_t cmd_queue);
void stopwatch_send_command(StopwatchCommand_t cmd);
uint32_t stopwatch_get_elapsed_seconds(void);




