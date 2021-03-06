#include "DisplayManager.h"
#include "Config.h"
#include "Clock.h"

int DisplayManager::cursor = 0;
int DisplayManager::state = DisplayState::Home;
int DisplayManager::savedState = DisplayState::Home;
container<DisplayItem> DisplayManager::items;

DisplayItem* DisplayManager::selected = NULL;


Ticker DisplayManager::sleepTimer;

void sleep_activate() {
	Serial.println("Activate sleep");
	DisplayManager::sleep();
}

int DisplayManager::init() {
	display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

	if (!display->begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
		Serial.println("SSD1306 allocation failed");
		return false;
	}

	display->clearDisplay();

	display->setTextSize(2); // Draw 2X-scale text
	display->setTextColor(SSD1306_WHITE);
	display->setCursor(23, 20);

	display->println(F("HomeAUT"));
	display->display();      // Show initial text
	delay(700);
}
void DisplayManager::drawMenu() {
	display->clearDisplay();
	display->setTextSize(2); // Draw 2X-scale text
	display->setTextColor(SSD1306_WHITE);

	display->drawRect(0, 21 * (cursor % 3), 128, 21, SSD1306_WHITE);
	
	int scroll_offset = cursor / 3;
	for (int k = scroll_offset*3; k < items.getSize() && k< scroll_offset*3+3; k++) {
		display->setCursor(10, k % 3 * 21+3);
		display->println(items.get(k)->getName());
	}
	display->display();
}

void DisplayManager::drawHome() {
	display->clearDisplay();
	display->setTextSize(2); // Draw 2X-scale text
	display->setTextColor(SSD1306_WHITE);
	display->setCursor(0, 20);
	display->println(Clock::getTime());
	display->display();
}

void DisplayManager::setCursor(int index) {
	if(state == DisplayState::Selected)
		selected->update(cursor-index, false);

	if(state == DisplayState::Menu){
		if (index >= items.getSize()) cursor = items.getSize() -1;
		else if (index < 0) cursor = 0;
		else cursor = index;
	}

	start_sleep_timer();
}

void DisplayManager::enter() {
	if (state == DisplayState::Sleep) state = savedState;
	else if (state == DisplayState::Home) state = DisplayState::Menu;
	else if (state == DisplayState::Menu) {
		selected = items.get(cursor);
		state = DisplayState::Selected;
	}
	else if (state == DisplayState::Selected) {
		selected->update(0, true);
	}
	else state = DisplayState::Menu;
	Serial.printf("Current mode: %d\n", state);
	start_sleep_timer();
}
void DisplayManager::back() {
	if(state == DisplayState::Selected)
		selected->update(0, false);

	if (state > 0) state -= 1;
}

void DisplayManager::update() {
	if (state == DisplayState::Menu) drawMenu();
	else if (state == DisplayState::Selected) selected->draw(display);
	else if (state == DisplayState::Sleep) { display->clearDisplay(); display->display(); }
	else if (state == DisplayState::Home) drawHome();
}

void DisplayManager::start_sleep_timer() {
	sleepTimer.detach();
	sleepTimer.once(SCREEN_TIMEOUT, sleep_activate);
}

void DisplayManager::sleep() {
	if (state == DisplayState::Sleep) return;
	savedState = state;
	state = DisplayState::Sleep;
}

