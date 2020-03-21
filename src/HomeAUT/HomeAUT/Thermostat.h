#pragma once

#include "TempSensor.h"

#define MIN_UPDATE_TIME 5000

class Thermostat : public DisplayItem
{
	TempSensor * tempSensor;
	int preferedTemp = 0;
	bool output = false;
	bool lastState = false;
	unsigned long elapsedTime = 0;
	int periodMode = 0;
	int fromDate = 0;
	int toDate = 0;
public:
	Thermostat(TempSensor* sensor) {
		tempSensor = sensor;
	}
	void check() {
		//if(tempSensor->getValue()[0])
	}

	char* getName() {
		return "THRMST";
	}

	virtual void draw(Adafruit_SSD1306* display) {
		display->clearDisplay();
		display->setCursor(0, 0);
		display->setTextSize(2);

		if (periodMode) {

			display->printf("SET: %d oC\n", preferedTemp);
			display->printf("FROM:%d:%d\n", fromDate / 60, fromDate-(fromDate/60)*60);
			display->printf("TO: %d:%d\n", toDate / 60, toDate - (toDate / 60) * 60);
		}
		else{
			display->printf("CUR: %d oC\n", tempSensor->getValue()[0]);
			display->printf("SET: %d oC\n", preferedTemp);
			display->printf("\nHeater: %d", output);
		}
		display->display();
	}
	virtual void update(int diff, boolean button) {
		
		if (button && periodMode == 0) periodMode = 1;
		else if( button &&  periodMode ==4 ) periodMode = 0;
		else if (button) {
			periodMode++;
		}

		if(periodMode ==0 || periodMode == 1){
			preferedTemp -= diff;
		}
		else if (periodMode == 2) {
			fromDate -= 15*diff;
			if (fromDate > 24 * 60) fromDate = 0;
			else if (fromDate < 0) fromDate = 24 * 60;
		}
		else if (periodMode == 3) {
			toDate -= 15*diff;
			if (toDate > 24 * 60) toDate = 0;
			else if (toDate < 0) toDate = 24 * 60;
		}
	}
};

