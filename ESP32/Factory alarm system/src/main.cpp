#include <WiFi.h>        
#include "DHTesp.h"
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <AudioFileSourceFS.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include <SPIFFS.h>

#define DHT11_PIN 18
#define GAS_PIN 34    
#define FIRE_PIN 35
DHTesp dht;

#define WIFI_SSID "SH3 P402"
#define WIFI_PASSWORD "studenthouse402"

const char* mqtt_server = "bf6a4b7bab9d450d96a5f70a6bc54040.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_username = "hoanganh";
const char* mqtt_password = "Hoanganh1";
char mqtt_message[128];

const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastMsg = millis();
unsigned long timeNow = 0;
#define MSG_BUFFER_SIRE (50)
char msg[MSG_BUFFER_SIRE];

AudioGeneratorMP3 *mp3;
AudioFileSourceFS *gasAudio;
AudioFileSourceFS *fireAudio;
AudioFileSourceFS *temperatureAudio;
int typeAudio;
AudioOutputI2S *out;

void setup_wifi(){
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(100);
    Serial.print(".");
  }
  Serial.println("WIFI connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_audio(){
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ Không thể mount SPIFFS.");
    return;
  }

  Serial.println("✅ SPIFFS đã sẵn sàng.");
  if (!SPIFFS.exists("/music.mp3")) {
    Serial.println("❌ Không tìm thấy file /music.mp3");
  }
  if (!SPIFFS.exists("/gas.mp3")) {
    Serial.println("❌ Không tìm thấy file /gas.mp3");
    return;
  }
  if (!SPIFFS.exists("/fire.mp3")) {
    Serial.println("❌ Không tìm thấy file /fire.mp3");
    return;
  }
  if (!SPIFFS.exists("/temperature.mp3")) {
    Serial.println("❌ Không tìm thấy file /temperature.mp3");
    return;
  }

  fireAudio = new AudioFileSourceFS(SPIFFS, "/fire.mp3");
  gasAudio = new AudioFileSourceFS(SPIFFS, "/gas.mp3");
  temperatureAudio = new AudioFileSourceFS(SPIFFS, "/temperature.mp3");
  out = new AudioOutputI2S();

  out->SetPinout(26, 25, 22);
  out->SetGain(0.4);

  mp3 = new AudioGeneratorMP3();
  mp3->begin(gasAudio, out);
  typeAudio = 3;
}

void reconnect(){
  while(!client.connected()){
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

void publishMessage(const char* topic, String payload, boolean retained) {
  if (client.publish(topic, payload.c_str(), retained))
    Serial.println("Message published [" + String(topic) + "]: " + payload);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) incommingMessage += (char)payload[i];
  Serial.println("Message arrived [" + String(topic) + "] " + incommingMessage);
}

void warning(float temp, int gas, int fire) {
  int newType = 0;

  if (gas > 900) {
    newType = 1;
  } else if (fire == 0) {
    newType = 2;
  } else if (temp > 40) {
    newType = 3;
  }

  if (newType != 0) {
    if (mp3 && mp3->isRunning()) {
      if (!mp3->loop()) {
        // Phát lại file nếu kết thúc
        switch (typeAudio) {
          case 1: gasAudio->seek(0, SEEK_SET); mp3->begin(gasAudio, out); break;
          case 2: fireAudio->seek(0, SEEK_SET); mp3->begin(fireAudio, out); break;
          case 3: temperatureAudio->seek(0, SEEK_SET); mp3->begin(temperatureAudio, out); break;
        }
      }
    } else {
      // Nếu chưa chạy, bắt đầu phát âm thanh mới
      switch (newType) {
        case 1: gasAudio->seek(0, SEEK_SET); mp3->begin(gasAudio, out); break;
        case 2: fireAudio->seek(0, SEEK_SET); mp3->begin(fireAudio, out); break;
        case 3: temperatureAudio->seek(0, SEEK_SET); mp3->begin(temperatureAudio, out); break;
      }
    }
    typeAudio = newType;
  } else {
    // Nếu mọi thứ an toàn, dừng âm thanh
    if (mp3 && mp3->isRunning()) {
      mp3->stop();
    }
    typeAudio = 0;
  }
}

void setup() {
  Serial.begin(9600);

  dht.setup(DHT11_PIN, DHTesp::DHT11);
  pinMode(FIRE_PIN, INPUT);
  setup_wifi();

  setup_audio();

  espClient.setCACert(root_ca); 
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if(!client.connected()){
    reconnect();
  }
  client.loop();

  timeNow = millis();
  if(timeNow - lastMsg > 3000){
    lastMsg = timeNow;

    float h = dht.getHumidity();
    float t = dht.getTemperature();
    int gasValue = analogRead(GAS_PIN);
    int fireDetected = digitalRead(FIRE_PIN);
    
    JsonDocument doc;
    doc["humidity"] = h;
    doc["temperature"] = t;
    doc["gas"] = gasValue;
    doc["fire"] =  fireDetected == 0 ? "Cháyyy" : "An toàn";
    serializeJson(doc, mqtt_message);
    publishMessage("esp32/factory/status", mqtt_message, true);

    warning(t, gasValue, fireDetected);
  }
}