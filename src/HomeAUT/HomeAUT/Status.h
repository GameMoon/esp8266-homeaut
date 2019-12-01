#pragma once
#include "DisplayItem.h"
#include "ESP8266WiFi.h"

class Status : public DisplayItem
{
	char* getName() {
		return "Status";
	}
	void draw(Adafruit_SSD1306* display) {
		display->clearDisplay();
		display->setTextSize(1);
		display->setCursor(0, 0);
		display->println("AP: ");
		display->println(WiFi.BSSIDstr());
		display->println("SoftAP: ");
		display->println(WiFi.BSSIDstr());
		display->display();
	}
	void update(int diff, boolean button) {}
};

