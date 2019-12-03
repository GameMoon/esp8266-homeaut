#pragma once

#include <Arduino.h>
#include "Config.h"

#define BOUNCE_DURATION 30
#define BACK_DURATION 400

class Encoder
{
public:
	static int switch_bounce;
	static uint16_t encoder_state;
	static int count;
	static void init();

};

