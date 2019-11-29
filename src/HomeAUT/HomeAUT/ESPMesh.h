#pragma once

#include <stdio.h>
#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>


#define ESP_OK 0
#define WIFI_CHANNEL 4
#define COMMAND_SIZE 6
#define MAC_SIZE 6
#define SEC_DELAY 312500

class ESPMesh{

public:

	struct message {
		uint8_t * src;
		uint8_t * dst;
		const uint8_t * command;
		uint8_t * payload;
		uint8_t payload_size;
	};
	static message* msg_buffer[];
	static uint8_t msg_buffer_size;

	static uint8_t broadcast_mac[];
	static uint8_t connection_tries;
	static boolean is_parent;
	static uint8_t* parent;
	
	
	static const uint8_t MSG_REQUEST_PARENT[];
	static const uint8_t MSG_ACCEPT_CHILD[];
	static const uint8_t MSG_ACCEPT_PARENT[];

	//static uint8_t softap_mac_address[MAC_SIZE];
	
	static ESPMesh::message* createMessage(uint8_t* to_addr, const uint8_t* command, uint8_t* payload = nullptr, uint8_t size = 0);
	static ESPMesh::message* parseMessage(uint8_t* data, uint8_t size);

	static int sendMessage(ESPMesh::message * msg);

	int setup_esp_now();
	void setup_connection();

	static void addMessage(message*);
	static message* popMessage();
	static void processMessage();
	
};
