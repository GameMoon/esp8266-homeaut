#pragma once

class Test : public DisplayItem {
	
	int testMode = 0;
	int start = 0;
	unsigned long startTime = 0;
	unsigned long endTime = 0;
	int sentPacket = 0;

	int numberOfPacket = 0;
	int sizeOfPacket = 0;

	int ok_transmit = 0;

	bool activate = false;

	char* getName() { return "Test"; }
	ESPMesh::message* msg = NULL;

	void draw(Adafruit_SSD1306* display) {
		display->clearDisplay();
		display->setCursor(0, 0);
		display->setTextSize(1);
		
		if(testMode < 0){
			display->printf("sleep");
			display->display();
			return;
		}

		display->printf("No. Packet: %d\n", numberOfPacket);
		display->printf("Size of Packet: %d\n", sizeOfPacket);


		if (activate) {
			display->println("Sending");
			if (sentPacket < numberOfPacket && !ESPMesh::is_sending) {
				
				msg->next_hop = ESPMesh::peers.get(0);
				msg->dst = msg->next_hop;

				int result = ESPMesh::sendMessage(msg,false);
				if (result == 0) ok_transmit++;
				display->printf("Sent packet %d state:%d\n", sentPacket,result);
				sentPacket++;
			}
			else if(sentPacket >= numberOfPacket){
				endTime = millis();
				activate = false;
			}
		}

		if(endTime != 0 && startTime != 0 && !activate){
			display->printf("Elapsed time: %lu\n",endTime - startTime);
			display->printf("%d / %d\n", ok_transmit, sentPacket);
			display->printf("Test counter: %d \n",NetworkManager::testCounter);
		}

		


		display->display();
	}

	void update(int diff, boolean button){
		testMode -= diff;

		if (button && testMode < 0) ESP.deepSleep(0);

		if (testMode % 3 == 0) {
			numberOfPacket = 10*pow10(testMode / 3);
			sizeOfPacket = 10;
		}
		else if (testMode % 3 == 1) {
			numberOfPacket = 10 * pow10(testMode / 3);
			sizeOfPacket = 100;
		}
		else if (testMode % 3 == 2) {
			numberOfPacket = 10 * pow10(testMode / 3);
			sizeOfPacket = 250-17;
		}
		

		if (button) {
			
			
			sentPacket = 0;
			ok_transmit = 0;
			NetworkManager::testCounter = 0;
			activate = button;
			startTime = millis();
		}
	}
public:
	Test() {
		uint8_t* payload = new	uint8_t[233];
		msg = ESPMesh::createMessage(NULL, NULL, ESPMesh::MSG_TEST, payload);
	}

};