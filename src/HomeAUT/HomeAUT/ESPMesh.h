#pragma once

#include <stdio.h>
#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>
#include "container.h"
#include "config.h"



class ESPMesh {

public:

	struct message {
		uint8_t* src;
		uint8_t* dst;
		uint8_t* next_hop;
		const uint8_t* command;
		uint8_t* payload;
		uint8_t payload_size;
	};

	struct status_entry {
		uint8_t *addr;
		uint8_t timeout;
	};
	
	static message* msg_buffer[];
	static uint8_t msg_buffer_size;

	static uint8_t broadcast_mac[];
	static uint8_t connection_tries;
	static boolean is_parent;
	static boolean is_root;
	static uint8_t* parent;

	static container<uint8_t> peers;
	static container<ESPMesh::status_entry> timeout_table;

	static const uint8_t MSG_REQUEST_PARENT[];
	static const uint8_t MSG_ACCEPT_CHILD[];
	static const uint8_t MSG_ACCEPT_PARENT[];
	static const uint8_t MSG_STATUS[];
	static const uint8_t MSG_UPDATE[];

	static ESPMesh::message* createMessage(uint8_t* next_hop, uint8_t* to_addr, const uint8_t* command, uint8_t* payload = nullptr, uint8_t size = 0);
	static ESPMesh::message* parseMessage(uint8_t* data, uint8_t size);

	static int sendMessage(ESPMesh::message* msg, bool delete_after = true);
	static void deleteMessage(ESPMesh::message* msg);

	int setup_esp_now();
	static void setup_connection();

	static void addMessage(message*);
	static message* popMessage();

	static void addPeer(uint8_t* addr);
	static void removePeer(uint8_t* addr);
	static void setParent(uint8_t* addr);
	static container<uint8_t>* getPeers() { return &peers; }


};
