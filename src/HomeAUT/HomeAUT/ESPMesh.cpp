#include "ESPMesh.h"

uint8_t ESPMesh::broadcast_mac[] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };
//uint8_t ESPMesh::broadcast_mac[] = { 0x1A,0xFE,0x34,0xDF,0x10,0xC8  }; //COM10
//uint8_t ESPMesh::broadcast_mac[] = { 0x5E,0xCF,0x7F,0x8C,0x0D,0x24 }; //COM8

const uint8_t ESPMesh::MSG_REQUEST_PARENT[COMMAND_SIZE] = { 0x52,0x45,0x51,0x50,0x52,0x54 };
const uint8_t  ESPMesh::MSG_ACCEPT_CHILD[COMMAND_SIZE] = { 0x41,0x43,0x50,0x43,0x48,0x44 };
const uint8_t  ESPMesh::MSG_ACCEPT_PARENT[COMMAND_SIZE] = { 0x41,0x43,0x50,0x50,0x52,0x54 };

ESPMesh::ESPMesh() {
	WiFi.softAPmacAddress(ESPMesh::softap_mac_address);	
}

void msg_recv_cb(uint8_t* mac_addr, uint8_t* data, uint8_t len) {
	Serial.println("recv_cb----");
}

void msg_send_cb(uint8_t* macAddr, uint8_t status) {
	Serial.println("send_cb");
	//Serial.printf("MAC address = %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
	Serial.println(status);
}

ESPMesh::message ESPMesh::createMessage(uint8_t* to_addr, const uint8_t* command, uint8_t* payload, uint8_t size) {
	uint8_t msg_length = sizeof(uint8_t) * (3 * 6 + size);
	uint8_t* msg = (uint8_t*)malloc(msg_length);
	uint8_t* from_addr = ESPMesh::softap_mac_address;

	memcpy(msg, from_addr, 6);
	memcpy(msg + 6, to_addr, 6);
	memcpy(msg + 12, command, 6);
	memcpy(msg + 18, payload, size);

	return message { msg, msg_length };
}

int ESPMesh::sendMessage(message msg) {
	//Serial.println(msg.size);
	int result = esp_now_send(ESPMesh::broadcast_mac, msg.data, msg.size);
	if (result == ESP_OK) {
		free(msg.data);
	}
	return result;
}


int ESPMesh::setup_esp_now() {
	WiFi.mode(WIFI_AP_STA);
	WiFi.disconnect();

	if (esp_now_init() != ESP_OK) {
		Serial.println("esp_now init failed");
		return -1;
	}

	esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
	esp_now_add_peer(broadcast_mac, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);

	
	if (esp_now_register_recv_cb(msg_recv_cb) != ESP_OK) {
		Serial.println("esp_now recv cb adding failed");
		return -1;
	}

	if (esp_now_register_send_cb(msg_send_cb) != ESP_OK) {
		Serial.println("esp_now send cb adding failed");
		return -1;
	}

	return ESP_OK;
}

void msg_send_cb(uint8_t* macAddr, uint8_t status) {
	Serial.println("send_cb");
	//Serial.printf("MAC address = %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
	Serial.println(status);
}

void ESPMesh::find_parent() {

}