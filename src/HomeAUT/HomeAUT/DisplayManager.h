#pragma once

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DisplayItem.h"
#include "container.h"
#include <Ticker.h>


enum DisplayState{ Sleep, Home, Menu, Selected };

class DisplayManager
{
private:
	static int cursor;
	Adafruit_SSD1306* display;
	static int state;
	static void start_sleep_timer();
	void drawMenu();
	void drawHome();
	static int savedState;
	static DisplayItem* selected;

public:
	static container<DisplayItem> items;
	static Ticker sleepTimer;

	int init();
	static void setCursor(int index);
	static int getCursor(){ return cursor; }
	static void enter();
	static void back();
	void update();
	static void sleep();

};

