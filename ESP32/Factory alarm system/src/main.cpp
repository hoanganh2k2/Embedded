#include <WiFi.h>        
#include "DHTesp.h"
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <AudioFileSourceFS.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include <SPIFFS.h>

#define DHT11_PIN 19
#define GAS_PIN 39
#define FIRE_PIN 18
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
AudioFileSourceFS *fireAudio;
AudioFileSourceFS *minGasAudio;
AudioFileSourceFS *maxGasAudio;
AudioFileSourceFS *minTemperatureAudio;
AudioFileSourceFS *maxTemperatureAudio;
AudioFileSourceFS *minHumidityAudio;
AudioFileSourceFS *maxHumidityAudio;
int typeAudio;
AudioOutputI2S *out;

float min_Temperature;
float max_Temperature;
float min_Gas;
float max_Gas;
float min_Humidity;
float max_Humidity;
JsonDocument config;

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

  fireAudio = new AudioFileSourceFS(SPIFFS, "/fire.mp3");
  minGasAudio = new AudioFileSourceFS(SPIFFS, "/minGas.mp3");
  maxGasAudio = new AudioFileSourceFS(SPIFFS, "/maxGas.mp3");
  minTemperatureAudio = new AudioFileSourceFS(SPIFFS, "/minTemperature.mp3");
  maxTemperatureAudio = new AudioFileSourceFS(SPIFFS, "/maxTemperature.mp3");
  minHumidityAudio = new AudioFileSourceFS(SPIFFS, "/minHumidity.mp3");
  maxHumidityAudio = new AudioFileSourceFS(SPIFFS, "/maxHumidity.mp3");
  out = new AudioOutputI2S();

  out->SetPinout(26, 25, 22);
  out->SetGain(0.5);

  mp3 = new AudioGeneratorMP3();
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

void loadConfig() {
  File file = SPIFFS.open("/config.json", "r");
  if (!file) {
    Serial.println("❌ Không tìm thấy file config.");
    return;
  }

  DeserializationError error = deserializeJson(config, file);

  if (error) {
    Serial.print("❌ Lỗi khi đọc file config: ");
    Serial.println(error.c_str());
    file.close();
    return;
  }

  min_Temperature = config["min_Temperature"];
  max_Temperature = config["max_Temperature"];
  min_Gas = config["min_Gas"];
  max_Gas = config["max_Gas"];
  min_Humidity = config["min_Humidity"];
  max_Humidity = config["max_Humidity"];

  file.close();
}

void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) incommingMessage += (char)payload[i];
  Serial.println("Message arrived [" + String(topic) + "] " + incommingMessage);

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, incommingMessage);
  if(error){
    Serial.println("❌ Failed to parse incoming JSON: " + String(error.c_str()));
    return;
  }

  JsonObject obj = doc.as<JsonObject>();
  if(obj["getConfig"].is<int>()){
      JsonDocument config;
      config["min_Temperature"] = min_Temperature;
      config["max_Temperature"] = max_Temperature;
      config["min_Gas"] = min_Gas;
      config["max_Gas"] = max_Gas;
      config["min_Humidity"] = min_Humidity;
      config["max_Humidity"] = max_Humidity;
      serializeJson(config, mqtt_message);
      publishMessage("esp32/config", mqtt_message, true);
  }

  if(obj["setConfig"].is<int>()){
    min_Temperature = obj["min_Temperature"];
    max_Temperature = obj["max_Temperature"];
    min_Gas = obj["min_Gas"];
    max_Gas = obj["max_Gas"];
    min_Humidity = obj["min_Humidity"];
    max_Humidity = obj["max_Humidity"];

    JsonDocument newConfig;
    newConfig["min_Temperature"] = min_Temperature;
    newConfig["max_Temperature"] = max_Temperature;
    newConfig["min_Gas"] = min_Gas;
    newConfig["max_Gas"] = max_Gas;
    newConfig["min_Humidity"] = min_Humidity;
    newConfig["max_Humidity"] = max_Humidity;

    File configFile = SPIFFS.open("/config.json", FILE_WRITE);
    if(!configFile) Serial.println("❌ Không thể mở config.json");
    else {
      if (serializeJson(newConfig, configFile) == 0) {
        Serial.println("❌ Ghi config.json thất bại.");
      }else {
        Serial.println("✅ Cập nhật config.json thành công.");
      }
      configFile.close();
    }
  }
}

void warning(float temp, int gas, int fire, float humidity) {
  int newType = 0;
  if (fire == 0) newType = 1;
  else if (gas < min_Gas) newType = 2;
  else if (gas > max_Gas) newType = 3;
  else if (temp < min_Temperature) newType = 4;
  else if (temp > max_Temperature) newType = 5;
  else if (humidity < min_Humidity) newType = 6;
  else if (humidity > max_Humidity) newType = 7;

  if (newType != 0 && newType != typeAudio) {
    switch (newType) {
      case 1: fireAudio->seek(0, SEEK_SET); mp3->begin(fireAudio, out); break;
      case 2: minGasAudio->seek(0, SEEK_SET); mp3->begin(minGasAudio, out); break;
      case 3: maxGasAudio->seek(0, SEEK_SET); mp3->begin(maxGasAudio, out); break;
      case 4: minTemperatureAudio->seek(0, SEEK_SET); mp3->begin(minTemperatureAudio, out); break;
      case 5: maxTemperatureAudio->seek(0, SEEK_SET); mp3->begin(maxTemperatureAudio, out); break;
      case 6: minHumidityAudio->seek(0, SEEK_SET); mp3->begin(minHumidityAudio, out); break;
      case 7: maxHumidityAudio->seek(0, SEEK_SET); mp3->begin(maxHumidityAudio, out); break;
    }
    typeAudio = newType;
  } else if (newType == 0 && mp3 && mp3->isRunning()) {
    typeAudio = 0;
  }
}

void setup() {
  Serial.begin(9600);

  dht.setup(DHT11_PIN, DHTesp::DHT11);
  pinMode(FIRE_PIN, INPUT);
  setup_wifi();

  espClient.setCACert(root_ca); 
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  delay(1000);
  setup_audio();
  if(!client.connected()){
    reconnect();
  }
  client.loop();
  loadConfig();
}

void loop() {
  if(!client.connected()){
    reconnect();
  }
  client.loop();

  if (mp3 && mp3->isRunning()) {
    if (!mp3->loop()) {
      switch (typeAudio) {
        case 1: fireAudio->seek(0, SEEK_SET); mp3->begin(fireAudio, out); break;
        case 2: minGasAudio->seek(0, SEEK_SET); mp3->begin(minGasAudio, out); break;
        case 3: maxGasAudio->seek(0, SEEK_SET); mp3->begin(maxGasAudio, out); break;
        case 4: minTemperatureAudio->seek(0, SEEK_SET); mp3->begin(minTemperatureAudio, out); break;
        case 5: maxTemperatureAudio->seek(0, SEEK_SET); mp3->begin(maxTemperatureAudio, out); break;
        case 6: minHumidityAudio->seek(0, SEEK_SET); mp3->begin(minHumidityAudio, out); break;
        case 7: maxHumidityAudio->seek(0, SEEK_SET); mp3->begin(maxHumidityAudio, out); break;
      }
    }
  }

  timeNow = millis();
  if(timeNow - lastMsg > 1500){
    lastMsg = timeNow;

    float h = dht.getHumidity();
    float t = dht.getTemperature();
    int gasValue = analogRead(GAS_PIN);
    int fireDetected = digitalRead(FIRE_PIN);
    
    warning(t, gasValue, fireDetected, h);

    JsonDocument doc;
    doc["humidity"] = h;
    doc["temperature"] = t;
    doc["gas"] = gasValue;
    doc["fire"] =  fireDetected == 0 ? "Cháyyy" : "An toàn";
    doc["warning"] = typeAudio;
    serializeJson(doc, mqtt_message);
    publishMessage("esp32/factory/status", mqtt_message, true);
  }
}