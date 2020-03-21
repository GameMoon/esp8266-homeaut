#pragma once
#include "DisplayItem.h"
#include "NetworkItem.h"
#include "Config.h"
#include <lm75.h>

class TempSensor : public DisplayItem, public NetworkItem
{
	TempI2C_LM75* sensor;
public:
	TempSensor() {
		sensor = new TempI2C_LM75(0x48, TempI2C_LM75::eleven_bits);
	}
	~TempSensor() {
		delete sensor;
	}

	char* getName() {
		return "Temp";
	}
	void draw(Adafruit_SSD1306* display) {
		display->clearDisplay();
		display->setTextSize(2);
		display->setCursor(10,10);
		display->printf("Temp: %f\n", sensor->getTemp());
		display->display();
	}
	void update(int diff, boolean button) {}

	uint8_t getType() { return SENSOR_TYPE_TEMP; }

	void updateValue() {
		uint16_t temp = sensor->getTemp();
		value[1] = (temp >> 8);
		value[0] = temp & 0xff;
	}

};


