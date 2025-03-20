#include <ESP8266WiFi.h>
const char* ssid = "SH3 P40200";
const char* password = "studenthouse402";

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_AP);
  IPAddress staticip(192,168,1,2);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  WiFi.softAPConfig(staticip, gateway, subnet);
  WiFi.softAP(ssid, password);

  delay(5000);
  Serial.println();
  Serial.print("IP host: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  Serial.print("Number of devices connected to wifi: ");
  Serial.println(WiFi.softAPgetStationNum());
  delay(5000);
}
