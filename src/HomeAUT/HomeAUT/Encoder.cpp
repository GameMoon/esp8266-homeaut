#include "Encoder.h"
#include "DisplayManager.h"

void ICACHE_RAM_ATTR encoder_detect() {
	if (digitalRead(ENCODER_PIN_B) == HIGH) {
		Encoder::count++;
		#ifdef USE_DISPLAY
			DisplayManager::setSelected(DisplayManager::getSelected() + 1);
		#endif // USE_
	}
	else {
		Encoder::count--;
		#ifdef USE_DISPLAY
			DisplayManager::setSelected(DisplayManager::getSelected() -1 );
		#endif // USE_
	}
}

int Encoder::count = 0;

void Encoder::init() {
	pinMode(ENCODER_PIN_A, INPUT);
	pinMode(ENCODER_PIN_B, INPUT);
	attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoder_detect, RISING);
}
