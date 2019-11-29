#pragma once
#include "DisplayItem.h"

class TempSensor : public DisplayItem
{
	char* getName() {
		return "Temp";
	}
	void draw(Adafruit_SSD1306* display) {
	}
};


