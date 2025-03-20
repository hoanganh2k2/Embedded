#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin("SH3 P402", "studenthouse402");

  delay(5000);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connedted!");
  Serial.print("IP station host:" );
  Serial.println(WiFi.localIP());

  WiFi.softAP("MyHome", "123123123");
  Serial.print("IP AP host: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  Serial.print("Number of devices connected to wifi: ");
  Serial.println(WiFi.softAPgetStationNum());
  delay(5000);
}
