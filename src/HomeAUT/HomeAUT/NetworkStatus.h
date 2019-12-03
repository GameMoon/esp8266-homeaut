#pragma once
#include "DisplayItem.h"
#include "ESP8266WiFi.h"
#include "ESPMesh.h"

class NetworkStatus : public DisplayItem
{

	int page = 0;

	char* getName() {
		return "Network";
	}

	void drawMACAddresses(Adafruit_SSD1306* display) {
		display->println("AP: ");
		display->println(WiFi.macAddress());
		display->println("SoftAP: ");
		display->println(WiFi.softAPmacAddress());
	}
	void drawParentStatus(Adafruit_SSD1306* display) {
		display->print("Parent: ");
		if (ESPMesh::parent != NULL) {
			for (int l = 0; l < MAC_SIZE; l++)  display->printf("%02x:", ESPMesh::parent[l]);
			display->println();
		}
		else display->println("NULL");

		display->printf("Is Parent: %d\n", ESPMesh::is_parent);
		display->printf("Is Root: %d\n", ESPMesh::is_root);
		display->printf("WiFi Connected: %d\n", WiFi.status());
	}

	void drawChildrenStatus(Adafruit_SSD1306* display) {
		display->printf("Peers: %d\n", ESPMesh::peers.getSize());

		for (int k = 0; k < ESPMesh::peers.getSize(); k++) {
			for (int l = 0; l < MAC_SIZE; l++)  display->printf("%02x:", ESPMesh::peers.get(k)[l]);
			display->println();
		}
	}

	void drawTimeout(Adafruit_SSD1306* display) {
		for (int k = 0; k < ESPMesh::timeout_table.getSize(); k++) {
			ESPMesh::status_entry* node = ESPMesh::timeout_table.get(k);

			
			for (int l = 0; l < 3; l++)  display->printf("%02x:", node->addr[l]);
			display->printf(" = %d", node->timeout);
			display->println();

		}
	}

	void draw(Adafruit_SSD1306* display) {
		display->clearDisplay();
		display->setTextSize(1);
		display->setCursor(0, 0);
		display->printf("Page: %d\n", page);
		display->setCursor(0, 20);

		if (page == 0) drawMACAddresses(display);
		else if (page == 1) drawParentStatus(display);
		else if (page == 2) drawChildrenStatus(display);
		else if (page == 3) drawTimeout(display);

		display->display();

	}

	void update(int diff, boolean button) {
		page -= diff;
	}
};

