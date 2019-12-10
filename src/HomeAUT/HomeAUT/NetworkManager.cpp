#include "NetworkManager.h"

container<NetworkItem> NetworkManager::items;
int NetworkManager::elapsed_time = 0;
int NetworkManager::timeout_elapsed_time = 0;

int NetworkManager::testCounter = 0;

uint8_t* NetworkManager::own_addr = NULL;

NetworkManager::NetworkManager() {
	mqtt_client = NULL;
	WiFi.softAPmacAddress(own_addr);
}

NetworkManager::~NetworkManager() {
	delete mqtt_client;
}

uint8_t* NetworkManager::createStatusPayload() {

	//Create payload
	uint8_t* payload = new uint8_t[getStatusPayloadSize() ];
	int offset = 0;
	for (int k = 0; k < items.getSize(); k++) 
	{
		payload[offset] = items.get(k)->getType();
		payload[offset + 1] = k;
		memcpy(payload + offset+2, items.get(k)->getValue(), SENSOR_PACKET_SIZE);
		offset += SENSOR_PACKET_SIZE + SENSOR_PACKET_TYPE + SENSOR_INDEX;
	}

	Serial.println();

	return payload;
}

uint8_t NetworkManager::getStatusPayloadSize() {
	return items.getSize() * (SENSOR_PACKET_SIZE + SENSOR_PACKET_TYPE + SENSOR_INDEX);
}

void NetworkManager::sendStatus() {
	if (millis() - elapsed_time < STATUS_REPORT_PERIOD) return;
	else elapsed_time = millis();

	ESPMesh::message * msg = ESPMesh::createMessage(NULL,NULL, ESPMesh::MSG_STATUS, createStatusPayload(),getStatusPayloadSize());
	if (ESPMesh::is_root) sendMQTT(msg);
	sendToPeers(msg);
}

void NetworkManager::sendToPeers(ESPMesh::message * msg,uint8* exclude_addr) {
	container<uint8_t>* peers = ESPMesh::getPeers();

	for (int k = 0; k < peers->getSize(); k++) {

		msg->dst = ESPMesh::broadcast_mac;
		msg->next_hop = peers->get(k);

		if (exclude_addr == NULL || memcmp(msg->next_hop, exclude_addr, MAC_SIZE) != 0){
			ESPMesh::sendMessage(msg,false);
		}

	}
	ESPMesh::deleteMessage(msg);
}

void NetworkManager::processMessage() {
	if (ESPMesh::msg_buffer_size == 0) return;
	//if (ESPMesh::msg_buffer.getSize() == 0) return;

	ESPMesh::message* msg = ESPMesh::popMessage();

	Serial.print("RECV command: ");
	for (int k = 0; k < COMMAND_SIZE; k++) {
		Serial.print((char)msg->command[k]);
	}
	Serial.println();

	if (memcmp(msg->command, ESPMesh::MSG_REQUEST_PARENT, COMMAND_SIZE) == 0 && ESPMesh::is_parent) { // child -> to everybody
		ESPMesh::sendMessage(ESPMesh::createMessage(msg->src,msg->src, ESPMesh::MSG_ACCEPT_CHILD));
		Serial.print("send accept_child");
	}
	else if (memcmp(msg->command, ESPMesh::MSG_ACCEPT_CHILD, COMMAND_SIZE) == 0 && ESPMesh::parent == NULL){ // parent -> child
		ESPMesh::setParent(msg->src);
		ESPMesh::sendMessage(ESPMesh::createMessage(msg->src,msg->src, ESPMesh::MSG_ACCEPT_PARENT));
	}
	else if (memcmp(msg->command, ESPMesh::MSG_ACCEPT_PARENT, COMMAND_SIZE) == 0) { // child -> parent
		ESPMesh::addPeer(msg->src);
		Serial.println("Child added");
	}
	else if (memcmp(msg->command, ESPMesh::MSG_STATUS, COMMAND_SIZE) == 0) { // status
		
		for (int k = 0; k < msg->payload_size; k++) {
			Serial.printf("%02x ", msg->payload[k]);
		}
		Serial.println();


		if (ESPMesh::is_root) sendMQTT(msg);

		

		updateTimeout(msg->src); 	//update timeout table
		//forwardMessage(msg); // forward message, if not came from parent
	
	}
	else if (memcmp(msg->command, ESPMesh::MSG_TEST, COMMAND_SIZE) == 0) { // update
		testCounter++;
	}
}



void NetworkManager::updateTimeout(uint8_t * addr) {
	if (ESPMesh::timeout_table.getSize() == 0) return;

	for (int k = 0; k < ESPMesh::timeout_table.getSize(); k++) {
		ESPMesh::status_entry* node = ESPMesh::timeout_table.get(k);

		if (memcmp(addr, node->addr, MAC_SIZE) == 0) {
			node->timeout = 0;
			return;
		}
	}
}

void NetworkManager::checkConnections() {
	if (millis() - timeout_elapsed_time < TIMEOUT_CHECK_PERIOD) return;
	else timeout_elapsed_time = millis();

	if (ESPMesh::timeout_table.getSize() == 0) return;
	
	for (int k = 0; k < ESPMesh::timeout_table.getSize(); k++) {
		ESPMesh::status_entry* node = ESPMesh::timeout_table.get(k);

		if (node->timeout >= 3) {
			ESPMesh::removePeer(node->addr);
			if (ESPMesh::parent != NULL && memcmp(node->addr, ESPMesh::parent, MAC_SIZE) == 0) {
				ESPMesh::parent = NULL;
				ESPMesh::setup_connection();
			}
		}
		else node->timeout++;
	}
}

void NetworkManager::forwardMessage(ESPMesh::message* msg) {
	if (memcmp(msg->dst, own_addr, MAC_SIZE) == 0) return;
	else {
		sendToPeers(msg, msg->src); // send to everybody except the source
	}
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {

	Serial.print("Message arrived in topic: ");
	Serial.println(topic);

	Serial.print("Message:");
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}

	Serial.println();
	Serial.println("-----------------------");

}
void NetworkManager::sendMQTT(ESPMesh::message* msg) {
	if (root_state != network_state::READY) return;
	uint8_t* payload = new uint8_t[msg->payload_size];
	memcpy(payload, msg->payload, msg->payload_size);
	
	uint8_t id[3] = { msg->src[0], msg->src[1], msg->src[2] };
	char topic[17];
	char value[9];

	for (int k = 0; k < msg->payload_size; k += SENSOR_PACKET_SIZE + SENSOR_PACKET_TYPE + SENSOR_INDEX) {
		sprintf(topic, "esp/%02x:%02x:%02x/%d/%d\0", id[0], id[1], id[2], payload[k], payload[k + 1]);
		sprintf(value, "%02x%02x%02x%02x\0", payload[k + 2], payload[k + 3], payload[k + 4], payload[k + 5]);
		mqtt_client->publish((const char*)& topic, (const char*)& value);
	}
	delete payload;

}

void NetworkManager::processRoot() {
	if (!ESPMesh::is_root) return;
	if (WiFi.status() != WL_CONNECTED && root_state == network_state::NO_CONNECTION) {
		WiFi.begin(ROOT_WIFI_SSID, ROOT_WIFI_PASSWORD);
		Serial.println("MQTT:Connecting to WiFi");
		root_state = network_state::WIFI_CONNECTING;
	}
	
	if (WiFi.status() == WL_CONNECTED && root_state == network_state::WIFI_CONNECTING) {
		Serial.println("MQTT::init");
		mqtt_client = new PubSubClient(wifi_client);
		mqtt_client->setServer(MQTT_SERVER, MQTT_PORT);
		mqtt_client->setCallback(mqtt_callback);

		root_state = network_state::MQTT_INIT;
	}
	if (root_state == network_state::MQTT_INIT && !mqtt_client->connected()) {
		mqtt_client->connect("ESP8266", MQTT_USER, MQTT_PASSWORD);
	}
	if (root_state == network_state::MQTT_INIT && mqtt_client->connected()) {
		Serial.println("MQTT::Connected to server");
		
		root_state = network_state::READY;
	}
	if (root_state == network_state::READY) {
		//mqtt_client->public
		
	}
	
}