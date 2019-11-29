#pragma once

#include "DisplayItem.h"

class Outputs : public DisplayItem
{
	char* getName() {
		return "Outputs";
	}
	void draw(Adafruit_SSD1306* display) {
	}
};

