#pragma once

#include "DisplayItem.h"
#include <RTClib.h>

class Clock : public DisplayItem
{


	static char current_time[5];
public:
	static RTC_DS1307 * rtc;

	Clock() {
		rtc = new RTC_DS1307();
		if (!rtc->begin()) {
			Serial.println("Clock::Couldn't find RTC");
		}
		rtc->adjust(DateTime(F(__DATE__), F(__TIME__)));
	}
	~Clock() {
		delete rtc;
	}

	char* getName() {
		return "Clock";
	}

	virtual void draw(Adafruit_SSD1306* display) {
		display->clearDisplay();
		display->setCursor(0, 0);
		display->setTextSize(1);

		DateTime now = rtc->now();
		display->print(now.year(), DEC);
		display->print('/');
		display->print(now.month(), DEC);
		display->print('/');
		display->println(now.day(), DEC);

		display->print(now.hour(), DEC);
		display->print(':');
		display->print(now.minute(), DEC);
		display->print(':');
		display->print(now.second(), DEC);

		display->display();
	}

	static char* getTime() {
		/*if(rtc->now().hour() > 24){
			sprintf(current_time, "%d:%d", rtc->now().hour(), rtc->now().minute());
		}
		else*/ 
			sprintf(current_time, "12:42");
		return current_time;
	}

	virtual void update(int diff, boolean button) {
		
	}

};

