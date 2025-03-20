#include <ESP8266WiFi.h>

const char* ssid = "SH3 P402";
const char* password = "studenthouse402";

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  IPAddress staticip(192,168,1,99);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  WiFi.config(staticip, gateway, subnet);
  WiFi.begin(ssid, password);

  delay(5000);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected!");
  Serial.print("IP host: ");
  Serial.println(WiFi.localIP());

  Serial.print("IP gateway: ");
  Serial.println(WiFi.gatewayIP());

  Serial.print("IP subnetMAsk: ");
  Serial.println(WiFi.subnetMask());
}

void loop() {
}
