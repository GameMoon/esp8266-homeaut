#pragma once

#include "DisplayItem.h"
#include "NetworkItem.h"

#define NUMBER_OF_GPIO 7

class GPIOState : public DisplayItem, public NetworkItem
{
	int gpio_selector = 0;
	boolean activate = false;
	int gpio_pins[NUMBER_OF_GPIO] = { 16,14,12,13,15,2,0 };
	int output_pins[NUMBER_OF_GPIO] = { 1,0,0,0,1,1,1 };

	char* getName() {
		return "GPIO";
	}
	
	void draw(Adafruit_SSD1306* display) {
		display->clearDisplay();
		display->setCursor(0, 0);
		display->setTextSize(1);

		for (int k = 0; k < 7; k++) {
			display->printf("P%d = %d\n", gpio_pins[k], digitalRead(gpio_pins[k]));
		}
		display->display();
	}

	void update(int diff, boolean button) {
		gpio_selector += diff;
		activate = button;
	}

	uint8_t getType() {
		return SENSOR_TYPE_GPIO;
	}

	void updateValue() {
		value[0] = 0;
		for (int k = 0; k < 7; k++) {
			value[0] |= (digitalRead(gpio_pins[k]) << k);
		}
	}

	void setValue(uint8_t* newValue) {
		int newStates = newValue[0];
		for (int k = 0; k < NUMBER_OF_GPIO; k++) {
			if (output_pins[k] == 1) {
				int state = (newValue[0] >> k) && 0x01;
				digitalWrite(gpio_pins[k], state);
			}
		}
	}
public:
	GPIOState() {
		for (int k = 0; k < NUMBER_OF_GPIO; k++) {
			if (output_pins[k] == 1) pinMode(gpio_pins[k], OUTPUT);
		}
	}
};

