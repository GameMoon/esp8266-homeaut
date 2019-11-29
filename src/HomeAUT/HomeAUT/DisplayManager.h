#pragma once

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DisplayItem.h"

class DisplayManager
{
private:
	static int menuSelector;
	Adafruit_SSD1306* display;
	DisplayItem** items;
	static int number_of_items;
public:
	DisplayManager();
	int init();
	void updateMenu();
	static void setSelected(int index);
	static int getSelected(){ return menuSelector; }
	void addItem(DisplayItem* item);
};

