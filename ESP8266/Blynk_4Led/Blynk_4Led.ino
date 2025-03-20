#define BLYNK_TEMPLATE_ID "TMPL6GS37T-gm"
#define BLYNK_TEMPLATE_NAME "4LED"
#define BLYNK_AUTH_TOKEN "QjVDC0dLmUiRMUJs7GI_HQtZa1ljj0XE"
#define BLYNK_TEMPLATE_VERSION "0.1.0"

#define BLYNK_PRINT Serial
#define APP_DEBUG
#define USE_NODE_MCU_BOARD

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define LED_RED   D1  // LED đỏ
#define LED_YELLOW D3 // LED vàng
#define LED_WHITE  D5 // LED trắng
#define LED_BLUE   D7 // LED xanh

#define WIFI_SSID "SH3 P402"
#define WIFI_PASSWORD "studenthouse402"

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_WHITE, LOW);
  digitalWrite(LED_BLUE, LOW);
}

BLYNK_WRITE(V0) {
  int state = param.asInt(); 
  digitalWrite(LED_RED, state);
}

BLYNK_WRITE(V1) {
  int state = param.asInt();
  digitalWrite(LED_YELLOW, state);
}

BLYNK_WRITE(V2) {
  int state = param.asInt();
  digitalWrite(LED_WHITE, state);
}

BLYNK_WRITE(V3) {
  int state = param.asInt();
  digitalWrite(LED_BLUE, state);
}

void loop() {
  Blynk.run();
}
