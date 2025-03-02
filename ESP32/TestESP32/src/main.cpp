#include <Arduino.h>

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(9600);
  pinMode(22, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(22, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(22, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
  Serial.print("Hello");
}