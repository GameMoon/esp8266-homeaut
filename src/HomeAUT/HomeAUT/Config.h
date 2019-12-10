#pragma once

#define ESP_OK 0
#define SEC_DELAY 312500

//Network
#define USE_ESPNOW
#define ESPNOW_SETUP_ON_START 1
#define SENSOR_PACKET_SIZE 4
#define SENSOR_PACKET_TYPE 1
#define SENSOR_INDEX 1
#define STATUS_REPORT_PERIOD 5000
#define TIMEOUT_CHECK_PERIOD 5000

#define WIFI_CHANNEL 4
#define MESH_PREFIX "ESPMesh"
#define MESH_PASSWORD "8t4SGSgx"
#define COMMAND_SIZE 6
#define MAC_SIZE 6
#define SHORT_MAC_SIZE 3

//ROOT Node accesss to Internet
#define ROOT_WIFI_SSID "gmhost"
#define ROOT_WIFI_PASSWORD "Valami12"

//MQTT
//#define USE_MQTT
#define MQTT_SERVER "farmer.cloudmqtt.com"
#define MQTT_PORT 11855
#define MQTT_USER "bzqikjef"
#define MQTT_PASSWORD "aS5VFuKnFURQ"



//Sensor types
#define SENSOR_TYPE_ADC 1
#define SENSOR_TYPE_TEMP 2
#define SENSOR_TYPE_GPIO 3

//Display
#define USE_DISPLAY
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_TIMEOUT 120

//Encoder
#define USE_ENCODER
#define ENCODER_PIN_A 13
#define ENCODER_PIN_B 12
#define ENCODER_PIN_SWITCH 14