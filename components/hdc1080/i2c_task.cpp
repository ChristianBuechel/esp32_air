
#include "i2c_task.h"
#include "hdc1080.h"
#include "io_wrapper.h"
#include <driver/i2c.h>

#define TAG "I2C_TASK"

#define HDC1080_ADR 0x40
#define SDA_PIN GPIO_NUM_26
#define SCL_PIN GPIO_NUM_27

int i2c_initialized = -1;

QueueHandle_t *i2c_queue;
i2c_config_t conf;
cHDC1080 hdc1080; // just initialize the object

static uint32_t millis()
{
    return esp_timer_get_time() / 1000;
}

static void send_event(float temp, float hum)
{
    i2c_event_t event = {};
    event.temp = temp;
    event.hum  = hum;
    event.time = millis();

    xQueueSend(i2c_queue, &event, portMAX_DELAY);
}

static void i2c_task(void *pvParameter)
{
    float temperature;
    float humidity;
    uint16_t conv_time;
    while (1)
    {
        conv_time = hdc1080.request_data();
        ESP_LOGI(TAG, "Conv Time %d",conv_time);
        conv_time = conv_time/100;
        if (conv_time < 11) {conv_time = 11;} //a minimum of one tick (10ms)
        vTaskDelay(conv_time / portTICK_PERIOD_MS);
    if (hdc1080.read_both(&temperature, &humidity))   
    {
       send_event(temperature, humidity);
    } 
    }
}

QueueHandle_t *i2c_init()
{

    if (i2c_initialized != -1)
    {
        ESP_LOGE(TAG, "Already initialized");
        return NULL;
    }

    i2c_initialized = 1;

    hdc1080.setup(I2C_NUM_0, HDC1080_ADR, SCL_PIN, SDA_PIN, I2C_FREQ_100K);


    // Initialize global state and queue
    i2c_queue = (void **)xQueueCreate(4, sizeof(i2c_event_t));
    ESP_LOGI(TAG, "Queue created");

    // Spawn a task for i2c
    xTaskCreate(&i2c_task, "i2c_task", 4096, NULL, 5, NULL);
    return i2c_queue;
}
