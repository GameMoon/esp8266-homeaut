#pragma once
#include "DisplayItem.h"
#include "NetworkItem.h"
#include "Config.h"
#include <Temperature_LM75_Derived.h>

class TempSensor : public DisplayItem, public NetworkItem
{
	Generic_LM75 * sensor;
public:
	TempSensor() {
		sensor = new Generic_LM75();
	}
	~TempSensor() {
		delete sensor;
	}

	char* getName() {
		return "Temp";
	}
	void draw(Adafruit_SSD1306* display) {
		display->clearDisplay();
		display->setTextSize(1);
		display->setCursor(10,10);
		display->printf("Temp: %f\n",sensor->readTemperatureC());
		display->display();
	}
	void update(int diff, boolean button) {}

	uint8_t getType() { return SENSOR_TYPE_TEMP; }

	void updateValue() {
		uint16_t temp = sensor->convertFloatTemperature(sensor->readTemperatureC());
		value[1] = temp & 0xff;
		value[0] = (temp >> 8);
	}

};


