#include "stopwatch.h"
#include <stdbool.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#ifdef __cplusplus
}
#endif

static SemaphoreHandle_t stopwatch_mutex = NULL;

static uint32_t counter = 0;
static bool running = false;
static QueueHandle_t stopwatch_cmd_queue = NULL;


static void stopwatch_task(void *pvParameters)
{
    StopwatchCommand_t cmd;
    TickType_t last_wake = xTaskGetTickCount();

    while (1)
    {
        
        if (xQueueReceive(stopwatch_cmd_queue, &cmd,0) == pdTRUE)
        {
            switch (cmd)
            {
                case STOPWATCH_CMD_START:
                if (xSemaphoreTake(stopwatch_mutex, portMAX_DELAY))
                {
                running = true;
                xSemaphoreGive(stopwatch_mutex);
               }

                 break;

                case STOPWATCH_CMD_STOP:
                    running = false;
                    break;

                case STOPWATCH_CMD_RESET:
                    counter = 0;
                    break;
            }
        }
        else
        {
            
           if (xSemaphoreTake(stopwatch_mutex, portMAX_DELAY)) {
           if (running)
           counter++;
           xSemaphoreGive(stopwatch_mutex);
        }

        }
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(1000));
    }
}

void stopwatch_init(QueueHandle_t cmd_queue)
{
    stopwatch_cmd_queue = cmd_queue;
    stopwatch_mutex = xSemaphoreCreateMutex();
    
    xTaskCreate(
        stopwatch_task,
        "Stopwatch",
        2048,
        NULL,
        1,
        NULL
    );
}

void stopwatch_send_command(StopwatchCommand_t cmd)
{
    if (stopwatch_cmd_queue != NULL)
        xQueueSend(stopwatch_cmd_queue, &cmd, 0);
}

uint32_t stopwatch_get_elapsed_seconds(void)
{
    uint32_t value = 0;

    if (xSemaphoreTake(stopwatch_mutex, portMAX_DELAY)) {
        value = counter;
        xSemaphoreGive(stopwatch_mutex);
    }

    return value;
}
