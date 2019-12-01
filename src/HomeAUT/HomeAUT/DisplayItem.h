#pragma once

#include <Adafruit_SSD1306.h>


class DisplayItem
{
	public:
		virtual char* getName() = 0;
		virtual void draw(Adafruit_SSD1306 * display) = 0;
		virtual void update(int diff, boolean button);
};





