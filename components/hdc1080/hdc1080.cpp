/*

Libraray for HDC1080 humidity and temperatiure sensor based on 

Arduino Library for Texas Instruments HDC1080 Digital Humidity and Temperature Sensor
Written by AA for ClosedCube
---

*/

#include "hdc1080.h"
#include "io_wrapper.h"

#define RAW2HUM(raw) (((float)raw) * 100 / 65536)
#define RAW2TEMP(raw) (((float)raw) * 165 / 65536 - 40)

cHDC1080::cHDC1080()
{
	//setResolution(HDC1080_RESOLUTION_14BIT, HDC1080_RESOLUTION_14BIT);
}

void cHDC1080::setup(uint8_t bus, uint8_t adr, uint8_t scl, uint8_t sda, uint32_t freq)
{
	h_bus = bus; //save stuff in object
	h_adr = adr;
	h_scl = scl;
	h_sda = sda;
	h_freq = freq;
	i2c_init(h_bus, h_scl, h_sda, h_freq); //initialize i2c
	set_resolution(hdc1080_14bit, hdc1080_14bit);
}

void cHDC1080::set_resolution(hdc1080_measurement_bitwidth_t humidity, hdc1080_measurement_bitwidth_t temperature)
{
	hdc1080_registers_t reg;
	reg.humidity_bitwidth = humidity;
	reg.temperature_bitwidth = temperature;
	h_res_temp = temperature;
	h_res_hum = humidity;
	set_registers(reg);
}

uint16_t cHDC1080::request_data()
{
	uint16_t wait = 0;
	hdc1080_registers_t registers = get_registers();
	registers.acquisition_mode = 1;
	set_registers(registers); // and ask for data
	switch (h_res_temp)
		{
		case hdc1080_11bit:
			wait = 365; //3.65ms
			break;
		case hdc1080_14bit:
			wait = 635; //6.35ms
			break;
		}
	switch (h_res_hum)
		{
		case hdc1080_8bit:
			wait += 250; //3.65ms
			break;
		case hdc1080_11bit:
			wait += 385; //3.65ms
			break;
		case hdc1080_14bit:
			wait += 650; //6.35ms
			break;
		}

	return wait;
}

void cHDC1080::heater_on()
{
	hdc1080_registers_t registers = get_registers();
	registers.heater = 1;
	set_registers(registers); // and ask for data
}

void cHDC1080::heater_off()
{
	hdc1080_registers_t registers = get_registers();
	registers.heater = 0;
	set_registers(registers); // and ask for data
}

uint16_t cHDC1080::get_wait()
{
	return 20; //ms for conversion --> make res depednet
}

uint16_t cHDC1080::get_manufacturer_id()
{
	return _read(HDC1080_MANUFACTURER_ID);
}

uint16_t cHDC1080::get_device_id()
{
	return _read(HDC1080_DEVICE_ID);
}

float cHDC1080::read_temperature()
{
	uint16_t rawT = _read(HDC1080_TEMPERATURE);
	return RAW2TEMP(rawT);
}

float cHDC1080::read_humidity()
{
	uint16_t rawH = _read(HDC1080_HUMIDITY);
	return RAW2HUM(rawH);
}

bool cHDC1080::read_both(float *temperature, float *humidity)
{
	uint8_t raw[4];
	uint16_t result;
	if (!_read_data(HDC1080_TEMPERATURE, (uint8_t *)&raw, 4, 20))
	{
		return false;
	}

	result = raw[0] << 8 | raw[1];
	*temperature = RAW2TEMP(result);
	result = raw[2] << 8 | raw[3];
	*humidity = RAW2HUM(result);
	return true;
}

void cHDC1080::set_registers(hdc1080_registers_t registers)
{
	uint8_t reg = HDC1080_CONFIGURATION;
	if (i2c_slave_write(h_bus, h_adr, &reg, (uint8_t *)&(registers.raw), sizeof(registers.raw)))
	{
	}
	vTaskDelay(10 / portTICK_PERIOD_MS);
}

hdc1080_registers_t cHDC1080::get_registers()
{
	hdc1080_registers_t reg;
	reg.raw = _read(HDC1080_CONFIGURATION);
	return reg;
}

uint16_t cHDC1080::_read(uint8_t reg)
{
	uint8_t result[2];
	if (!_read_data(reg, (uint8_t *)&result, 2, 10))
	{
		return 0;
	}
	return (uint16_t)(result[0] << 8) | result[1];
}

bool cHDC1080::_read_data(uint8_t reg, uint8_t *data, uint32_t len, TickType_t delay)
{

	if (i2c_slave_write(h_bus, h_adr, &reg, NULL, 0))
	{
		return false;
	}

	vTaskDelay(delay / portTICK_PERIOD_MS);

	if (i2c_slave_read(h_bus, h_adr, NULL, data, len))
	{
		return false;
	}

	return true;
}
