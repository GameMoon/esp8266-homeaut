#pragma once

#include "DisplayItem.h"

class GPIO : public DisplayItem
{
	int gpio_selector = 0;
	boolean activate = false;

	char* getName() {
		return "Outputs";
	}
	
	void draw(Adafruit_SSD1306* display) {
		display->clearDisplay();
		display->setCursor(10, 10);
		display->setTextSize(1);
		display->printf("GPIO: %d", gpio_selector);
		display->display();
	}

	void update(int diff, boolean button) {
		gpio_selector += diff;
		activate = button;
	}
};

