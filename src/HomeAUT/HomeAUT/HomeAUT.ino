/*
 Name:		HomeAUT.ino
 Created:	11/13/2019 3:47:14 PM
 Author:	daniel
*/


#include "Config.h"
#include "ESPMesh.h"
#include "Encoder.h"
#include "DisplayManager.h"
#include "Outputs.h"
#include "TempSensor.h"
#include "Status.h"
#include <Ticker.h>

#ifdef USE_ESPNOW
int esp_now_status = -1;
ESPMesh mesh;
#endif

#ifdef USE_DISPLAY
DisplayManager dManager;
#endif

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	pinMode(LED_BUILTIN, OUTPUT);

	#ifdef USE_ESPNOW
			esp_now_status = mesh.setup_esp_now(); //ESP-NOW setup
			if (esp_now_status != ESP_OK) Serial.println("esp_now setup failed");
			Serial.print("SoftAP mac: "); Serial.println(WiFi.softAPmacAddress());  
	#endif

	#ifdef USE_DISPLAY
		dManager.init();
	#endif
	
	#ifdef ESPNOW_SETUP_ON_START
		mesh.setup_connection(); //Search for parent node
	#endif

	#ifdef USE_ENCODER
		Encoder encoder;
		encoder.init();
	#endif // USE_ENCODER

	dManager.addItem(new Outputs);
	dManager.addItem(new TempSensor());
	dManager.addItem(new Status());
	
	pinMode(ENCODER_PIN_SWITCH, INPUT);
}



// the loop function runs over and over again until power down or reset
void loop() {
	//Serial.println(digitalRead(ENCODER_PIN_SWITCH));

	dManager.update();
	delay(100);
}
