/* Air sensors
*/

#include "hdc1080.h"
#include "SerialCommand.h"
#include "uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "i2c_task.h"


#define TAG "MAIN"

extern "C"
{
	void app_main();
}

template <class T>
T check_range(T value, T min, T max)
{
	if (value < min)
		return min;
	if (value > max)
		return max;
	return value;
}


bool is_diff(int32_t a, int32_t b, int32_t range)
{
	return (a/range) != (b/range);
}


SerialCommand myCMD; // The  SerialCommand object

void unrecognized(const char *command)
{
	char *arg;
	arg = myCMD.next(); // Get the next argument from the SerialCommand object buffer
	if (arg != NULL)	// As long as it existed, take it
	{
	    ESP_LOGI(TAG, "> %s;%s", command,arg);
	}
	else
	{
	    ESP_LOGI(TAG, "> %s", command);
	}
}

void process_h0()
{
		ESP_LOGI(TAG, "Heater OFF");
		hdc1080.heater_off();
}

void process_h1()
{
		ESP_LOGI(TAG, "Heater ON");
		hdc1080.heater_on();
}

void process_dev_id()
{
		ESP_LOGI(TAG, "Device ID: 0x%02x",hdc1080.get_device_id());
}

void process_man_id()
{
		ESP_LOGI(TAG, "Manufacturer ID: 0x%02x",hdc1080.get_manufacturer_id());
}

void app_main()
{
	//esp_log_level_set("*", ESP_LOG_NONE); //can be changed using UART cmd LOG;0 or 1
	esp_log_level_set("*", ESP_LOG_INFO); //can be changed using UART cmd LOG;0 or 1

	//setup hdc1080
	i2c_event_t i2cev;
	QueueHandle_t i2c_queue = i2c_init(); //init with GPIOs would be nice

	//initialite UART
	uart_init();
	myCMD.setDefaultHandler(unrecognized);	//
	myCMD.addCommand("h1", process_h1);	//
	myCMD.addCommand("h0", process_h0);	//
	myCMD.addCommand("dev_id", process_dev_id);	//
	myCMD.addCommand("man_id", process_man_id);	//
	myCMD.clearBuffer();


	while (1) //main loop
	{

		myCMD.readSerial();
		if (xQueueReceive(i2c_queue, &i2cev, 0)) //do not wait
		{
			ESP_LOGI(TAG, "Last reading(us): %d", uint32_t(i2cev.time));
            ESP_LOGI(TAG, "Temperature     : %f", i2cev.temp);
            ESP_LOGI(TAG, "Humidity        : %f", i2cev.hum);
		}

		vTaskDelay(1000 / portTICK_RATE_MS); //
	}
}