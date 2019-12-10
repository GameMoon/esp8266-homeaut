#include "Encoder.h"
#include <Ticker.h>
#include "DisplayManager.h"


void ICACHE_RAM_ATTR encoder_detect() {

	if (millis() - Encoder::encoder_state < 30) return;
	else Encoder::encoder_state = millis();

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
}

int Encoder::count = 0;
unsigned long Encoder::switch_bounce = 0;
unsigned long Encoder::encoder_state = 0;
/*
unsigned long Encoder::elapsed_time = 0;
uint8_t Encoder::encoder_state = 0;

unsigned long Encoder::last_switch_check = 0;
unsigned long Encoder::now = 0;

//uint8_t Encoder::enc_table[] = { 0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0 };
uint8_t Encoder::enc_table[] = { 0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0 };

void update_enc() {
	Encoder::now = millis();
	Serial.printf("delta time: %d\n", Encoder::now - Encoder::elapsed_time);

	if (Encoder::now - Encoder::elapsed_time > 1) {
		Encoder::encoder_state = (Encoder::encoder_state << 2) & 0x0F;
		if (digitalRead(ENCODER_PIN_A) == LOW) {
			Encoder::encoder_state |= 2;
		}
		if (digitalRead(ENCODER_PIN_B) == LOW) {
			Encoder::encoder_state |= 1;
		}
		Serial.printf("state: %d\n", Encoder::enc_table[Encoder::encoder_state]);
		Encoder::elapsed_time = Encoder::now;
	}

	if (Encoder::now - Encoder::last_switch_check > BOUNCE_DURATION) {

		if (Encoder::elapsed_time > BACK_DURATION && digitalRead(ENCODER_PIN_SWITCH))
			DisplayManager::back();
		else if (digitalRead(ENCODER_PIN_SWITCH) == HIGH) {
			DisplayManager::enter();
		}

		Encoder::last_switch_check = Encoder::now;
	}

}
*/
void Encoder::init() {
	pinMode(ENCODER_PIN_A, INPUT);
	pinMode(ENCODER_PIN_B, INPUT);
	pinMode(ENCODER_PIN_SWITCH, INPUT);

	attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoder_detect, RISING);
	attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_SWITCH), encoder_press, CHANGE);
}

