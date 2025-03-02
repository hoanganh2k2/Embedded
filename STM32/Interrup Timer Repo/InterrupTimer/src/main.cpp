#include <Arduino.h>
#include <HardwareTimer.h>

#define LED1 PA0
#define LED2 PA2
#define LED3 PA4

#define INTERVAL1 1000000
#define INTERVAL2 2000000
#define INTERVAL3 3000000

HardwareTimer Timer1(TIM2);
HardwareTimer Timer2(TIM3);
HardwareTimer Timer3(TIM4);

void ngat1()
{
  digitalWrite(LED1, !digitalRead(LED1));
}

void ngat2()
{
  digitalWrite(LED2, !digitalRead(LED2));
}

void ngat3()
{
  digitalWrite(LED3, !digitalRead(LED3));
}

void setup() {  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  Timer1.pause();
  Timer1.setOverflow(INTERVAL1, MICROSEC_FORMAT);
  Timer1.attachInterrupt(ngat1);
  Timer1.resume();

  Timer2.pause();
  Timer2.setOverflow(INTERVAL2, MICROSEC_FORMAT);
  Timer2.attachInterrupt(ngat2);
  Timer2.resume();

  Timer3.pause();
  Timer3.setOverflow(INTERVAL3, MICROSEC_FORMAT);
  Timer3.attachInterrupt(ngat3);
  Timer3.resume();
}

void loop() {
  delay(1000);
}