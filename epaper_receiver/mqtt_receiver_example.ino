/*
 * Baltic Creatures Display - MQTT Receiver for ESP32
 * 
 * This sketch receives creature information via MQTT and displays it on a 2.9" eInk display.
 * 
 * Required Libraries:
 * - PubSubClient (for MQTT)
 * - ArduinoJson (for JSON parsing)
 * - Waveshare 2.9" eInk library (included in epaper_receiver folder)
 * 
 * Configuration:
 * - Update WIFI_SSID and WIFI_PASSWORD
 * - Update MQTT_SERVER and MQTT_PORT
 * - Set DEVICE_ID to match your ESP32 (e.g., "esp32_1", "esp32_2")
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// WiFi Configuration
const char* WIFI_SSID = "your_ssid";
const char* WIFI_PASSWORD = "your_password";

// MQTT Configuration
const char* MQTT_SERVER = "192.168.1.100";  // Change to your MQTT broker IP
const int MQTT_PORT = 1883;
const char* DEVICE_ID = "esp32_1";          // Must match the device list in client.py

// Construct MQTT topic: creature/esp32_1
String topic;

WiFiClient espClient;
PubSubClient client(espClient);

struct Creature {
  String name;
  String info;
  String dangers;
};

Creature currentCreature;

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n\nBosnia Creatures Display - MQTT Receiver");
  
  // Initialize eInk display (placeholder - implement according to your display library)
  // initDisplay();
  
  connectWiFi();
  topic = "creature/" + String(DEVICE_ID);
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(onMqttMessage);
  connectMQTT();
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();
  delay(1000);
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    String clientId = "creature-display-" + String(DEVICE_ID);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(topic.c_str());
      Serial.print("Subscribed to: ");
      Serial.println(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  
  // Convert payload to string
  char message[length + 1];
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  
  // Parse JSON
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, message);
  
  if (error) {
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    return;
  }
  
  // Extract creature data
  currentCreature.name = doc["name"].as<String>();
  currentCreature.info = doc["info"].as<String>();
  currentCreature.dangers = doc["dangers"].as<String>();
  
  Serial.println("Creature data received:");
  Serial.println(currentCreature.name);
  Serial.println(currentCreature.info);
  Serial.println(currentCreature.dangers);
  
  // Display on eInk
  displayCreature();
}

void displayCreature() {
  /*
   * TODO: Implement display logic here
   * 
   * This depends on your eInk display library.
   * 
   * Example using typical eInk display:
   * 
   * display.setTextSize(2);
   * display.setTextColor(BLACK);
   * display.setCursor(5, 5);
   * display.println(currentCreature.name);
   * 
   * display.setTextSize(1);
   * display.setCursor(5, 35);
   * display.println("Info:");
   * display.println(currentCreature.info);
   * 
   * display.setCursor(5, 100);
   * display.println("Dangers:");
   * display.println(currentCreature.dangers);
   * 
   * display.display();  // Refresh eInk display
   */
  
  Serial.println("Display updated (placeholder)");
}
