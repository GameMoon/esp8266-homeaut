#pragma once

#include <Arduino.h>
#include "Config.h"

#define BOUNCE_DURATION 10
#define BACK_DURATION 400

class Encoder
{
public:
	static uint8_t enc_table[];
	//static unsigned long elapsed_time;
	//static unsigned long last_switch_check;
	//static uint8_t encoder_state;
	static unsigned long encoder_state;
	static unsigned long switch_bounce;

	static unsigned long now;

	static int count;
	static void init();
	static void update();

};

