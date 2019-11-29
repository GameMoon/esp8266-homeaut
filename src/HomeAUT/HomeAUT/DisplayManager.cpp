#include "DisplayManager.h"
#include "Config.h"

DisplayManager::DisplayManager() {
	number_of_items = 0;
}

int DisplayManager::menuSelector = 0;
int DisplayManager::number_of_items = 0;

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

void DisplayManager::updateMenu() {
	display->clearDisplay();
	display->setTextSize(2); // Draw 2X-scale text
	display->setTextColor(SSD1306_WHITE);
	display->setCursor(23, 20);

	display->drawRect(0, 21 * (menuSelector % 3), 128, 21, SSD1306_WHITE);
	
	int scroll_offset = menuSelector / 3;
	for (int k = scroll_offset*3; k < number_of_items && k< scroll_offset*3+3; k++) {
		display->setCursor(10, k % 3 * 21+3);
		display->println(items[k]->getName());
	}
	display->display();
}

void DisplayManager::setSelected(int index) {
	if (index > number_of_items) menuSelector = number_of_items;
	else if (index < 0) menuSelector = 0;
	else menuSelector = index;
}

void DisplayManager::addItem(DisplayItem* item) {
	DisplayItem** temp = new DisplayItem * [number_of_items + 1];
	memcpy(temp, items, number_of_items*sizeof(DisplayItem*));
	temp[number_of_items] = item;
	delete items;
	items = temp;
	number_of_items++;
}