/*#ifndef _I2C_TASK_H_
#define _I2C_TASK_H_ 

#ifdef __cplusplus
extern "C"
{
#endif 
*/

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <stdint.h>
#include "hdc1080.h"


typedef struct {
	float temp;
	float hum;
    uint32_t time;
} i2c_event_t;

QueueHandle_t *i2c_init();

extern cHDC1080 hdc1080; // make it available for main


/*
#ifdef __cplusplus
}
#endif

#endif 

*/