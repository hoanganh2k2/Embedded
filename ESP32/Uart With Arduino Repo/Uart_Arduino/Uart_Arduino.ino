#include <Arduino.h>
#include <SoftwareSerial.h>

#define RX_PIN 4
#define TX_PIN 5
#define LED_PIN 6

SoftwareSerial mySerial(RX_PIN, TX_PIN);

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  if(mySerial.available()){
    char c = mySerial.read();
    Serial.write(c);

    if(c == '0') digitalWrite(LED_PIN, LOW);
    else if( c == '1') digitalWrite(LED_PIN, HIGH);
    else Serial.print("Invalid character");
  }
}
