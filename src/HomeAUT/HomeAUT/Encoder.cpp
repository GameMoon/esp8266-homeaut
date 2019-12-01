#include "Encoder.h"
#include "DisplayManager.h"


void ICACHE_RAM_ATTR encoder_detect() {
	if (digitalRead(ENCODER_PIN_B) == HIGH) {
		Encoder::count++;
		#ifdef USE_DISPLAY
			DisplayManager::setCursor(DisplayManager::getCursor() + 1);
		#endif // USE_
	}
	else {
		Encoder::count--;
		#ifdef USE_DISPLAY
			DisplayManager::setCursor(DisplayManager::getCursor() -1 );
		#endif // USE_
	}
}

void ICACHE_RAM_ATTR encoder_press() {
	int elapsed_time = abs(Encoder::switch_bounce - millis());
	if (elapsed_time < BOUNCE_DURATION) return;
	

	if (elapsed_time > BACK_DURATION && digitalRead(ENCODER_PIN_SWITCH))
		DisplayManager::back();
	else if (digitalRead(ENCODER_PIN_SWITCH) == HIGH) {
		DisplayManager::enter();
	}

	Encoder::switch_bounce = millis();

	Serial.printf("Button pressed %d\n", elapsed_time);
	
}

int Encoder::count = 0;
int Encoder::switch_bounce = 0;
uint16_t Encoder::encoder_state = 0;

void Encoder::init() {
	pinMode(ENCODER_PIN_A, INPUT);
	pinMode(ENCODER_PIN_B, INPUT);
	pinMode(ENCODER_PIN_SWITCH, INPUT);

	attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoder_detect, RISING);
	attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_SWITCH), encoder_press, CHANGE);
}
