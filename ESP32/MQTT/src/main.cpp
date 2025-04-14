#include <WiFi.h>        
#include "DHTesp.h"
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

#define DHTpin 18
DHTesp dht;

#define WIFI_SSID "SH3 P402"
#define WIFI_PASSWORD "studenthouse402"

const char* mqtt_server = "bf6a4b7bab9d450d96a5f70a6bc54040.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_username = "hoanganh";
const char* mqtt_password = "Hoanganh1";

WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientID =  "ESPClient-";
    clientID += String(random(0xffff), HEX);
    if (client.connect(clientID.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("esp32/client");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) incommingMessage += (char)payload[i];
  Serial.println("Message arrived [" + String(topic) + "] " + incommingMessage);
}

void publishMessage(const char* topic, String payload, boolean retained) {
  if (client.publish(topic, payload.c_str(), retained))
    Serial.println("Message published [" + String(topic) + "]: " + payload);
}

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(1);

  dht.setup(DHTpin, DHTesp::DHT11);

  setup_wifi();
  espClient.setInsecure(); // Chỉ dùng nếu bạn không verify chứng chỉ
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

unsigned long timeUpdata = millis();

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (millis() - timeUpdata > 5000) {
    delay(dht.getMinimumSamplingPeriod());
    float h = dht.getHumidity();
    float t = dht.getTemperature();

    JsonDocument doc;
    doc["humidity"] = h;
    doc["temperature"] = t;
    char mqtt_message[128];
    serializeJson(doc, mqtt_message);
    publishMessage("esp32/dht11", mqtt_message, true);

    timeUpdata = millis();
  }
}
