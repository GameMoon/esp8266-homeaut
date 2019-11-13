/*
 Name:		HomeAUT.ino
 Created:	11/13/2019 3:47:14 PM
 Author:	daniel
*/


#include "ESPMesh.h"
#include <Ticker.h>

int esp_now_status = -1;
ESPMesh mesh;

Ticker blinker;

void changeState() {
	digitalWrite(LED_BUILTIN, !(digitalRead(LED_BUILTIN)));
}

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	pinMode(LED_BUILTIN, OUTPUT);

	//ESP-NOW setup
	esp_now_status = mesh.setup_esp_now();
	if (esp_now_status != ESP_OK) Serial.println("esp_now setup failed");
	Serial.print("SoftAP mac: "); Serial.println(WiFi.softAPmacAddress());

	blinker.attach(0.5, changeState);
}

// the loop function runs over and over again until power down or reset
void loop() {
	mesh.sendMessage(mesh.createMessage(ESPMesh::broadcast_mac, ESPMesh::MSG_REQUEST_PARENT));

	delay(1000);
}
