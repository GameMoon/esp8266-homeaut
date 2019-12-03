#pragma once

#include "ESPMesh.h"
#include "NetworkItem.h"
#include "container.h"
#include <PubSubClient.h>

enum network_state{NO_CONNECTION,WIFI_CONNECTING,MQTT_INIT,READY};



class NetworkManager
{
	static int elapsed_time;
	static int timeout_elapsed_time;

	uint8_t getStatusPayloadSize();
	int root_state;
	PubSubClient * mqtt_client;
	WiFiClient wifi_client;
	
	void forwardMessage(ESPMesh::message* msg);
	void updateTimeout(uint8_t * addr);
	void sendMQTT(ESPMesh::message* msg);
	void sendToPeers(ESPMesh::message* msg,uint8_t* exclude = NULL);
	void processStatusPayload();
	static uint8_t* own_addr;
public:
	NetworkManager();
	~NetworkManager();
	static container<NetworkItem> items;
	uint8_t* createStatusPayload();
	void checkConnections();
	void sendStatus();
	void processMessage();
	void processRoot();

};

