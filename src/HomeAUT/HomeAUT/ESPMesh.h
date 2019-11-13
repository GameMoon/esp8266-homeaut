#pragma once

#ifndef esp_mesh
#define esp_mesg

#include <stdio.h>
#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>


#define ESP_OK 0
#define WIFI_CHANNEL 4
#define COMMAND_SIZE 6
#define MAC_SIZE 6

class ESPMesh{
private:
	struct message {
		uint8_t* data;
		uint8_t size;
	};
public:
	static uint8_t broadcast_mac[];

	static const uint8_t MSG_REQUEST_PARENT[];
	static const uint8_t MSG_ACCEPT_CHILD[];
	static const uint8_t MSG_ACCEPT_PARENT[];

	uint8_t softap_mac_address[MAC_SIZE];
	
	message createMessage(uint8_t* to_addr, const uint8_t* command, uint8_t* payload = nullptr, uint8_t size = 0);
	int sendMessage(message msg);

	int setup_esp_now();
	void find_parent();

	ESPMesh();
};

#endif // !esp_mesh