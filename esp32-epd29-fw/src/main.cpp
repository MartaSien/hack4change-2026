// GxEPD2_HelloWorld.ino by Jean-Marc Zingg
//
// Display Library example for SPI e-paper panels from Dalian Good Display and
// boards from Waveshare. Requires HW SPI and Adafruit_GFX. Caution: the e-paper
// panels require 3.3V supply AND data lines!
//
// Display Library based on Demo Example from Good Display:
// https://www.good-display.com/companyfile/32/
//
// Author: Jean-Marc Zingg
//
// Version: see library.properties
//
// Library: https://github.com/ZinggJM/GxEPD2

// Supporting Arduino Forum Topics (closed, read only):
// Good Display ePaper for Arduino:
// https://forum.arduino.cc/t/good-display-epaper-for-arduino/419657 Waveshare
// e-paper displays with SPI:
// https://forum.arduino.cc/t/waveshare-e-paper-displays-with-spi/467865
//
// Add new topics in https://forum.arduino.cc/c/using-arduino/displays/23 for
// new questions and issues

// see GxEPD2_wiring_examples.h for wiring suggestions and examples
// if you use a different wiring, you need to adapt the constructor parameters!

// uncomment next line to use class GFX of library GFX_Root instead of
// Adafruit_GFX
// #include <GFX.h>

#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_4C.h>
#include <GxEPD2_7C.h>
#include <GxEPD2_BW.h>

// select the display class and display driver class in the following file (new
// style):
#include "GxEPD2_display_selection_new_style.h"

// alternately you can copy the constructor from GxEPD2_display_selection.h or
// GxEPD2_display_selection_added.h to here e.g. for Wemos D1 mini:
// GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT>
// display(GxEPD2_154_D67(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/
// 4)); // GDEH0154D67

// for handling alternative SPI pins (ESP32, RP2040) see example
// GxEPD2_Example.ino

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WifiManager.h>
#include <ezTime.h>

#include "bitmap.h"

#define MAX_PAYLOAD 1024
WiFiClient wifiClient;
PubSubClient client(wifiClient);

DynamicJsonDocument doc(MAX_PAYLOAD);

AsyncWebServer server(80);

Timezone tz;

unsigned long mqtt_reconnection_progress_millis = 0;
unsigned long one_minute = 60 * 1000;
unsigned long five_seconds = 5 * 1000;

const char *MQTT_BROKER = "mqtt.hs3";
const uint16_t MQTT_BROKER_PORT = 1883;
const char *MQTT_USER = "hs3";
const char *MQTT_PASSWORD = "iddqdidkfa";

const char *feedTopic = "feed/public_transport";
const char *switchSetTopic = "hs3/greetspace/tramwajomat/switch/set";
const char *client_id = "neptun";

void handleFeedUpdate(byte *payload, unsigned int length) {
  Serial.println("Handling feed update");
  deserializeJson(doc, payload, length);

  JsonArray ztm = doc["baltic"];
}

void onConnectionEstablished() {
  Serial.println("Connection established");
  client.subscribe(feedTopic);
  client.subscribe(switchSetTopic);
  client.setCallback(
      [](char *messageTopic, uint8_t *payload, unsigned int length) {
        if (strcmp(messageTopic, feedTopic) == 0) {
          handleFeedUpdate(payload, length);
        }
      });
}

void setupSerial() { Serial.begin(115200); }
void setupWifi() {
  bool connected = WifiManager.connectToWifi();
  if (!connected) {
    WifiManager.startManagementServer("Neptun");
  }
}

void setupMqtt() {
  client.setServer(MQTT_BROKER, MQTT_BROKER_PORT);
  client.setBufferSize(MAX_PAYLOAD);
}

int fact_idx = 0;
const int total_facts = 2;

char json[] = "{\
    \"zmieraczek\": {\
        \"name\": \"Zmieraczek Plażowy\",\
        \"info\": \"Objęty częściową ochroną w Polsce\",\
        \"dangers\": \"Sprzątanie plaż z kidziny, martwej materii organicznej\"\
    },\
    \"widlik\": {\
        \"name\": \"Widlik Zaostrzony\",\
        \"info\": \"Objęty ochroną ścisłą w Polsce\",\
        \"dangeres\": \"Jest wrażliwy na zanieczyszczenia wody\"\
    },\
    \"morszczyn\": {\
        \"name\": \"Morszczyn Pęcherzychowaty\",\
        \"info\": \"Objęty ścisłą ochroną w Polsce\", \
        \"dangers\": \"Zbyt mała przejrzystość wody spowodowana procesem eutrofizacji\"\
    }\
}";

const char HelloWorld[] = "Hello World!";

const char *fact, *info;

void draw() {
    #ifdef FALSE
  deserializeJson(doc, json);
    JsonObject::iterator it = doc.as<JsonObject>().begin();

    for (int i = 0; i < fact_idx; i++) 
        ++it;

  fact = it->value()["name"];
  info = it->value()["info"];
  fact_idx = (fact_idx + 1) % total_facts;
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby;
  uint16_t tbw, tbh;
  display.getTextBounds(fact, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center the bounding box by transposition of the origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.println(fact);
  display.setFont(&FreeSans9pt7b);
    display.println(info);
  } while (display.nextPage());
  #endif

  display.setFullWindow();
  display.firstPage();
  display.fillScreen(GxEPD_WHITE);
  if (fact_idx == 0) {
    display.drawImage(e1001, 0, 0, 128, 296, false, false, true);
  } else {
    display.drawImage(e1002, 0, 0, 128, 296, false, false, true);
  }
  fact_idx = (fact_idx + 1) % total_facts;
}

void setup() {
  setupSerial();
  //delay(2000);
  setupWifi();
  setupMqtt();

  tz.setLocation("Europe/Warsaw");
  //waitForSync(5);

  // display.init(115200); // default 10ms reset pulse, e.g. for bare panels
  // with DESPI-C02
  display.init(115200, true, 2,
               false); // USE THIS for Waveshare boards with "clever" reset
                       // circuit, 2ms reset pulse
}

bool timerIsClean() { return mqtt_reconnection_progress_millis == 0; }
bool timerIsTimedOut() {
  return (mqtt_reconnection_progress_millis > 0 &&
          millis() - mqtt_reconnection_progress_millis > five_seconds);
}

void reconnectMqtt() {
  if (timerIsClean() || timerIsTimedOut()) {
    Serial.println("Connecting to MQTT broker using supplied credentials");
    bool connected = client.connect(client_id, MQTT_USER, MQTT_PASSWORD);

    if (connected) {
      mqtt_reconnection_progress_millis = 0;
      onConnectionEstablished();
    } else {
      Serial.printf("MQTT connection failed, rc=%d, retry in 5 seconds\n",
                    client.state());
      mqtt_reconnection_progress_millis = millis();
    }
  }
}

void mqttLoop() { client.loop(); }

void loop() {
  WifiManager.check();

  if (!client.connected()) {
    reconnectMqtt();
  } else {
    mqttLoop();
  }

  Serial.println("Drawing");
  draw();

  display.hibernate();
  delay(1000);
};
