#include "ESPMesh.h"

uint8_t ESPMesh::broadcast_mac[] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };
//uint8_t ESPMesh::broadcast_mac[] = { 0x1A,0xFE,0x34,0xDF,0x10,0xC8  }; //COM10
//uint8_t ESPMesh::broadcast_mac[] = { 0x5E,0xCF,0x7F,0x8C,0x0D,0x24 }; //COM8

const uint8_t  ESPMesh::MSG_REQUEST_PARENT[COMMAND_SIZE] = { 0x52,0x45,0x51,0x50,0x52,0x54 };
const uint8_t  ESPMesh::MSG_ACCEPT_CHILD[COMMAND_SIZE] = { 0x41,0x43,0x50,0x43,0x48,0x44 };
const uint8_t  ESPMesh::MSG_ACCEPT_PARENT[COMMAND_SIZE] = { 0x41,0x43,0x50,0x50,0x52,0x54 };
const uint8_t  ESPMesh::MSG_STATUS[COMMAND_SIZE] = { 0x73,0x74,0x61,0x74,0x75,0x73 };
const uint8_t  ESPMesh::MSG_UPDATE[COMMAND_SIZE] = { 0x75,0x70,0x64,0x61,0x74,0x65 };
const uint8_t  ESPMesh::MSG_TEST[COMMAND_SIZE] = { 0x74,0x65,0x73,0x74,0x65,0x6b };

uint8_t ESPMesh::connection_tries = 0;
uint8_t ESPMesh::msg_buffer_size = 0;
bool ESPMesh::is_parent = false;
bool ESPMesh::is_root = false;
uint8_t* ESPMesh::parent = NULL;
bool ESPMesh::is_sending = false;

//container<ESPMesh::message> ESPMesh::msg_buffer;
ESPMesh::message* ESPMesh::msg_buffer[10];
container<uint8_t> ESPMesh::peers;
container<ESPMesh::status_entry> ESPMesh::timeout_table;

void msg_recv_cb(uint8_t* mac_addr, uint8_t* data, uint8_t len) {
	ESPMesh::addMessage(ESPMesh::parseMessage(data, len));
}

void msg_send_cb(uint8_t* macAddr, uint8_t status) {
	
		ESPMesh::is_sending = false;
	
}

ESPMesh::message* ESPMesh::createMessage(uint8_t* next_hop,uint8_t* to_addr, const uint8_t* command, uint8_t* payload, uint8_t size) {
	message* msg = new message();
	uint8 src[MAC_SIZE];
	WiFi.softAPmacAddress(src);

	msg->src = src;
	msg->dst = to_addr;
	msg->next_hop = next_hop;
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

int ESPMesh::sendMessage(message * msg, bool delete_after) {
	uint8_t data_size = 2*MAC_SIZE + COMMAND_SIZE + msg->payload_size;
	uint8_t* data = new uint8_t[data_size];
	
	memcpy(data, msg->src, MAC_SIZE);
	memcpy(data + MAC_SIZE, msg->dst, MAC_SIZE);
	memcpy(data + 2*MAC_SIZE, msg->command, COMMAND_SIZE);
	memcpy(data + 2*MAC_SIZE + COMMAND_SIZE, msg->payload, msg->payload_size);
	
	is_sending = true;
	int result = esp_now_send(msg->next_hop, data, data_size);

	if(result == ESP_OK && delete_after) {
		delete msg->payload;
		delete msg;
	}
	delete data;
	return result;
}

void ESPMesh::deleteMessage(message* msg) {
	delete msg->payload;
	delete msg;
}

int ESPMesh::setup_esp_now() {
	WiFi.mode(WIFI_AP_STA);
	WiFi.softAP(MESH_PREFIX, MESH_PASSWORD, WIFI_CHANNEL, 1);
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

	ESPMesh::sendMessage(ESPMesh::createMessage(ESPMesh::broadcast_mac,ESPMesh::broadcast_mac, ESPMesh::MSG_REQUEST_PARENT));

	uint8_t all_con, en_con;
	esp_now_get_cnt_info(&all_con, &en_con);

	if (ESPMesh::connection_tries < 5 && all_con < 2) { //reset if no connection or timeout
		
		ESPMesh::connection_tries++;
		timer1_write(SEC_DELAY*2); 
	}
	else if(all_con > 1){
		ESPMesh::is_parent = true;
		digitalWrite(LED_BUILTIN, LOW);
	}
	else if (ESPMesh::connection_tries >= 5) {
		ESPMesh::is_parent = true;
		ESPMesh::is_root = true;
	}
}

void ESPMesh::setup_connection() {
	ESPMesh::connection_tries = 0;
	timer1_attachInterrupt(findParent);
	timer1_enable(TIM_DIV256, TIM_EDGE, TIM_SINGLE);
	timer1_write(SEC_DELAY*2); 
}

void ESPMesh::addMessage(message* msg) {
	//msg_buffer.add(msg);
	ESPMesh::msg_buffer[ESPMesh::msg_buffer_size] = msg;
	ESPMesh::msg_buffer_size++;
}

ESPMesh::message* ESPMesh::popMessage() {
	if (ESPMesh::msg_buffer_size == 0) return 0;
	ESPMesh::message* currentMsg = ESPMesh::msg_buffer[0]; 
	ESPMesh::msg_buffer_size--;
	memmove(msg_buffer + 1, msg_buffer, ESPMesh::msg_buffer_size);
	return currentMsg;
	//return msg_buffer.get(0);
}

void ESPMesh::addPeer(uint8_t* addr) {
	if (esp_now_is_peer_exist(addr) != 0) return;

	esp_now_add_peer(addr, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);
	peers.add(addr);

	timeout_table.add(new ESPMesh::status_entry{ peers.get(peers.getSize() - 1),  0 });
}

void ESPMesh::setParent(uint8_t* addr) {
	ESPMesh::parent = addr;
	ESPMesh::addPeer(addr);
}
	

void ESPMesh::removePeer(uint8_t* addr) {

	Serial.println("ESPMesh::peer_removed");
	for (int l = 0; l < MAC_SIZE; l++)  Serial.printf("%02x:", addr[l]);
	Serial.println();

	if (esp_now_is_peer_exist(addr) == 0) return;
		esp_now_del_peer(addr);
	
	for (int k = 0; k < timeout_table.getSize(); k++) {
		if (memcmp(addr, timeout_table.get(k)->addr, MAC_SIZE) == 0) {
			timeout_table.delByIndex(k);
			break;
		}
	}
	peers.del(addr);
}
 