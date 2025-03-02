#include <Arduino.h>
#include <Wire.h>

#define ARDUINO_I2C_ADDR  6
#define BUTTON PA1
#define LED PA7

void interrup(){
  Wire.beginTransmission(ARDUINO_I2C_ADDR);
  digitalWrite(LED, !digitalRead(LED));
  if(digitalRead(LED) == HIGH) Wire.write('H');
  else Wire.write('L');
  Wire.endTransmission();
}

void setup() {
  Wire.begin();
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON), interrup, FALLING);
}

void loop() {
  delay(2000);
}