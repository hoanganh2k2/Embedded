#include <Arduino.h>
#define LED PA_4
#define BUTTON1 PA_7
#define BUTTON2 PA_0

void interrup(){
  digitalWrite(LED, !digitalRead(LED));
}

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  digitalWrite(LED, HIGH);

  attachInterrupt(digitalPinToInterrupt(BUTTON1),interrup, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON2),interrup, FALLING);
}

void loop() {
  delay(10000);
}