#pragma once

#include "Config.h"

class NetworkItem
{
protected:
	uint8_t* value;
public:
	NetworkItem() {
		value = new uint8_t[SENSOR_PACKET_SIZE];
	}
	~NetworkItem() {
		delete value;
	}
	virtual char* getName() = 0;
	virtual uint8_t* getValue() { updateValue();  return value; }
	virtual uint8_t getType() = 0;
	virtual void updateValue() = 0;
	virtual void setValue(uint8_t*) {}
};