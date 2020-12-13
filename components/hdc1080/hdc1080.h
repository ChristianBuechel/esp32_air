/*

Arduino Library for Texas Instruments HDC1080 Digital Humidity and Temperature Sensor
Written by AA for ClosedCube
---

The MIT License (MIT)

Copyright (c) 2016-2017 ClosedCube Limited

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#ifndef _hdc1080_h

#define _hdc1080_h

#include <stdint.h>

#define	HDC1080_TEMPERATURE		0x00
#define HDC1080_HUMIDITY		0x01
#define HDC1080_CONFIGURATION	0x02
#define HDC1080_MANUFACTURER_ID 0xFE
#define HDC1080_DEVICE_ID		0xFF
#define HDC1080_SERIAL_ID_FIRST	0xFB
#define HDC1080_SERIAL_ID_MID	0xFC
#define HDC1080_SERIAL_ID_LAST	0xFD


typedef enum {
	hdc1080_8bit = 0x02,
	hdc1080_11bit = 0x01,
	hdc1080_14bit = 0x00
} hdc1080_measurement_bitwidth_t;

typedef union {
	uint8_t raw[6];
	struct {
		uint16_t serialFirst;
		uint16_t serialMid;
		uint16_t serialLast;
	};
} hdc1080_serial_t;

typedef union {
	uint16_t raw;
	struct {
		uint8_t humidity_bitwidth : 2;
		uint8_t temperature_bitwidth : 1;
		uint8_t battery_status : 1;
		uint8_t acquisition_mode : 1;
		uint8_t heater : 1;
		uint8_t reserved_again : 1;
		uint8_t software_reset : 1;
        uint8_t unused;
	};
} hdc1080_registers_t;

class cHDC1080 {
public:
	cHDC1080(); //constructor
    void setup(uint8_t bus, uint8_t adr, uint8_t scl, uint8_t sda, uint32_t freq);
    void set_resolution(hdc1080_measurement_bitwidth_t humidity, hdc1080_measurement_bitwidth_t temperature);
    uint16_t request_data();
    void heater_on();
    void heater_off();
	uint16_t get_wait();
	uint16_t get_manufacturer_id();
	uint16_t get_device_id();
    float read_temperature();
	float read_humidity(); 
	bool read_both(float* temperature, float* humidity);
	void set_registers(hdc1080_registers_t registers);
	hdc1080_registers_t get_registers();

private:
uint16_t _read(uint8_t reg);
bool _read_data(uint8_t reg, uint8_t* data, uint32_t len, uint32_t delay);
	
protected:

uint8_t h_bus;
uint8_t h_adr;
uint8_t h_scl;
uint8_t h_sda;
uint32_t h_freq;

uint8_t h_res_temp;
uint8_t h_res_hum;

};

#endif

