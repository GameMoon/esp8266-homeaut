#pragma once

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DisplayItem.h"
#include <Ticker.h>


enum DisplayState{ Sleep, Home, Menu, Selected };

class DisplayManager
{
private:
	static int cursor;
	Adafruit_SSD1306* display;
	static DisplayItem** items;
	static int number_of_items;
	static int state;
	static void start_sleep_timer();
	void drawMenu();
	void drawHome();
	static int savedState;
	static DisplayItem* selected;
	static void setSelected(int index);

public:
	static Ticker sleepTimer;

	DisplayManager();
	int init();
	static void setCursor(int index);
	static int getCursor(){ return cursor; }
	void addItem(DisplayItem* item);
	static void enter();
	static void back();
	void update();
	static void sleep();
};

