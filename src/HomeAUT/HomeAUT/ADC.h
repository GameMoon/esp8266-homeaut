#pragma once

#include "DisplayItem.h"
#include "NetworkItem.h"
#include "Arduino.h"

class ADC : public DisplayItem, public NetworkItem
{

public:
	ADC() : NetworkItem(){
		pinMode(A0, INPUT);
	}

	char* getName() {
		return "ADC";
	}
	void draw(Adafruit_SSD1306* display) {
		display->clearDisplay();
		display->setTextSize(1);
		display->setCursor(10, 10);
		display->printf("ADC: %d\n", analogRead(A0));
		display->display();
	}
	void update(int diff, boolean button) {}

	void updateValue() {
		value[0] = analogRead(A0);
	}

	uint8_t getType() { return SENSOR_TYPE_ADC; }
};

