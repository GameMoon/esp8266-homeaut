#include "ESPMesh.h"

uint8_t ESPMesh::broadcast_mac[] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };
//uint8_t ESPMesh::broadcast_mac[] = { 0x1A,0xFE,0x34,0xDF,0x10,0xC8  }; //COM10
//uint8_t ESPMesh::broadcast_mac[] = { 0x5E,0xCF,0x7F,0x8C,0x0D,0x24 }; //COM8

const uint8_t ESPMesh::MSG_REQUEST_PARENT[COMMAND_SIZE] = { 0x52,0x45,0x51,0x50,0x52,0x54 };
const uint8_t  ESPMesh::MSG_ACCEPT_CHILD[COMMAND_SIZE] = { 0x41,0x43,0x50,0x43,0x48,0x44 };
const uint8_t  ESPMesh::MSG_ACCEPT_PARENT[COMMAND_SIZE] = { 0x41,0x43,0x50,0x50,0x52,0x54 };

uint8_t ESPMesh::connection_tries = 0;
uint8_t ESPMesh::msg_buffer_size = 0;
bool ESPMesh::is_parent = false;
uint8_t* ESPMesh::parent = NULL;

ESPMesh::message* ESPMesh::msg_buffer[10];

void msg_recv_cb(uint8_t* mac_addr, uint8_t* data, uint8_t len) {
	ESPMesh::addMessage(ESPMesh::parseMessage(data, len));
}

void msg_send_cb(uint8_t* macAddr, uint8_t status) {
}

ESPMesh::message* ESPMesh::createMessage(uint8_t* to_addr, const uint8_t* command, uint8_t* payload, uint8_t size) {
	message* msg = new message();
	uint8 src[MAC_SIZE];
	WiFi.softAPmacAddress(src);

	msg->src = src;
	msg->dst = to_addr;
	msg->command = command;
	msg->payload = payload;
	msg->payload_size = size;

	return msg;
}

ESPMesh::message* ESPMesh::parseMessage(uint8_t* data, uint8_t size) {
	message* msg = new message();
	msg->payload_size = size - 2*MAC_SIZE - COMMAND_SIZE;
	uint8_t* from_addr = new uint8_t[MAC_SIZE];
	uint8_t* to_addr = new uint8_t[MAC_SIZE];
	uint8_t* command = new uint8_t[COMMAND_SIZE];
	uint8_t* payload = new uint8_t[msg->payload_size];

	memcpy(from_addr, data, MAC_SIZE);
	memcpy(to_addr, data+MAC_SIZE, MAC_SIZE);
	memcpy(command, data+MAC_SIZE*2, COMMAND_SIZE);
	memcpy(payload, data+MAC_SIZE*2+COMMAND_SIZE, msg->payload_size);


	msg->src = from_addr;
	msg->dst = to_addr;
	msg->command = command;
	msg->payload = payload;

	return msg;
}

int ESPMesh::sendMessage(message * msg) {
	uint8_t data_size = 2*MAC_SIZE + COMMAND_SIZE + msg->payload_size;
	uint8_t* data = new uint8_t[data_size];
	
	memcpy(data, msg->src, MAC_SIZE);
	memcpy(data + MAC_SIZE, msg->dst, MAC_SIZE);
	memcpy(data + 2*MAC_SIZE, msg->command, COMMAND_SIZE);
	memcpy(data + 2*MAC_SIZE + COMMAND_SIZE, msg->payload, msg->payload_size);

	int result = esp_now_send(msg->dst, data, data_size);

	if(result == ESP_OK ) {
		delete msg->payload;
		delete msg;
	}
	delete data;
	return 0;
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

void ICACHE_RAM_ATTR findParent() {
	digitalWrite(LED_BUILTIN, !(digitalRead(LED_BUILTIN)));  //Toggle LED Pin

	ESPMesh::sendMessage(ESPMesh::createMessage(ESPMesh::broadcast_mac, ESPMesh::MSG_REQUEST_PARENT));

	uint8_t all_con, en_con;
	esp_now_get_cnt_info(&all_con, &en_con);

	if (ESPMesh::connection_tries < 5 && all_con < 2) { //reset if no connection or timeout
		
		ESPMesh::connection_tries++;
		timer1_write(SEC_DELAY*2); 
	}
	else {
		ESPMesh::is_parent = true;
		digitalWrite(LED_BUILTIN, HIGH);
	}
}

void ESPMesh::setup_connection() {
	ESPMesh::connection_tries = 0;
	timer1_attachInterrupt(findParent);
	timer1_enable(TIM_DIV256, TIM_EDGE, TIM_SINGLE);
	timer1_write(SEC_DELAY*2); 
}

void ESPMesh::addMessage(message* msg) {
	ESPMesh::msg_buffer[ESPMesh::msg_buffer_size] = msg;
	ESPMesh::msg_buffer_size++;
}

ESPMesh::message* ESPMesh::popMessage() {
	if (ESPMesh::msg_buffer_size == 0) return 0;
	ESPMesh::message* currentMsg = ESPMesh::msg_buffer[0]; 
	ESPMesh::msg_buffer_size--;
	memmove(msg_buffer + 1, msg_buffer, ESPMesh::msg_buffer_size);
	return currentMsg;
}

void ESPMesh::processMessage() {
	if (ESPMesh::msg_buffer_size == 0) return;

	ESPMesh::message* msg = ESPMesh::popMessage();

	Serial.print("RECV command: ");
	for (int k = 0; k < COMMAND_SIZE; k++) {
		Serial.print((char)msg->command[k]);
	}
	Serial.println();

	if (memcmp(msg->command, ESPMesh::MSG_REQUEST_PARENT, COMMAND_SIZE) == 0 && ESPMesh::is_parent) {
		ESPMesh::sendMessage(ESPMesh::createMessage(msg->src, ESPMesh::MSG_ACCEPT_CHILD));
	}
	if (memcmp(msg->command, ESPMesh::MSG_ACCEPT_CHILD, COMMAND_SIZE) == 0 && ESPMesh::parent == NULL) {
		ESPMesh::parent = msg->src;
		esp_now_add_peer(msg->src, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);
		ESPMesh::sendMessage(ESPMesh::createMessage(msg->src, ESPMesh::MSG_ACCEPT_PARENT));
	}
	if (memcmp(msg->command, ESPMesh::MSG_ACCEPT_PARENT, COMMAND_SIZE) == 0) {
		esp_now_add_peer(msg->src, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);
		Serial.println("Child added");
	}
}