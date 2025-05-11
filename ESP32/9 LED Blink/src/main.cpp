#include <Arduino.h>

#define LED1 23
#define LED2 22
#define LED3 16
#define LED4 4
#define LED5 21
#define LED6 19
#define LED7 18
#define LED8 5
#define LED9 17

const int ledPins[] = {23, 22, 16, 4, 21, 19, 18, 5, 17};

const int freq = 5000;
const int ledChannelBase = 0;
const int resolution = 8; // 8-bit -> giá trị từ 0 đến 255

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);
  pinMode(LED8, OUTPUT);
  pinMode(LED9, OUTPUT);

  for (int i = 0; i < 9; i++) {
    ledcSetup(ledChannelBase + i, freq, resolution);
    ledcAttachPin(ledPins[i], ledChannelBase + i);
  }
}

void ALL_OFF(int time_Delay){
  for(int i = 0; i < 9; i++){
    ledcWrite(ledChannelBase + i, 255);
  }
  delay(time_Delay);
}

void ALL_ON(int time_Delay){
  for(int i = 0; i < 9; i++){
    ledcWrite(ledChannelBase + i, 0);
  }
  delay(time_Delay);
}

void Blink(int ledPin, int time_delay){
  ledcWrite(ledChannelBase + ledPin, 0);
  delay(time_delay);
  ledcWrite(ledChannelBase + ledPin, 255);
}

void Round1(){
  for(int i = 0; i < 3; i++){
    ALL_ON(800);
    ALL_OFF(400);
  }
}

void Round2(){
  for(int i = 0; i < 9; i++){
    Blink(i, 200);
  }
  delay(500);
  for(int i = 8; i >= 0; i--){
    Blink(i, 200);
  }
  delay(500);
}

void Round3(){
  for(int i = 0; i < 9 ; i+=2){
    ledcWrite(ledChannelBase + i, 0);
    delay(50);
  }
  for(int i = 7; i > 0; i-=2){
    ledcWrite(ledChannelBase + i, 0);
    delay(50);
  }
}

void Round4(){
  delay(500);
  for(int i = 0; i < 9 ; i+=2){
    ledcWrite(ledChannelBase + i, 255);
    delay(200);
  }
  for(int i = 7; i > 0; i-=2){
    ledcWrite(ledChannelBase + i, 255);
    delay(200);
  }
}

void Round5(){
  delay(500);
  for(int i = 0; i < 9; i++){
    ledcWrite(ledChannelBase + i, 0);
    delay(200);
  }
  for(int i = 0; i < 2; i++){
    ALL_OFF(200);
    ALL_ON(200);
  }
}

void Round6(){
  delay(500);
  for(int i = 8; i >= 0; i--){
    ledcWrite(ledChannelBase + i, 255);
    delay(200);
  }
  for(int i = 0; i < 2; i++){
    ALL_ON(200);
    ALL_OFF(200);
  }
}

void Round7() {
  delay(500);
  for (int duty = 255; duty >= 0;) {
    for (int i = 0; i < 9; i++) {
      ledcWrite(ledChannelBase + i, duty);
    }
    if(duty < 155) duty -= 5;
    else duty -= 1;
    delay(50); //tốc độ sáng lên
  }
  delay(100);
  ALL_OFF(200);
  ALL_ON(200);
}

void Round8() {
  delay(500);
  for (int duty = 0; duty <= 255;) {
    for (int i = 0; i < 9; i++) {
      ledcWrite(ledChannelBase + i, duty);
    }
    if(duty < 155) duty += 5;
    else duty += 1;
    delay(50); //tốc độ sáng lên
  }
}

void Round9(){
  delay(700);

  for(int i = 0; i < 9; i++){
    if(i <= 4){
      for(int j = 4 - i; j <= 4 + i; j++){
        ledcWrite(ledChannelBase + j, 255 - (i+1 - abs(4-j)) * 51);
      }
    }else{
      ledcWrite(ledChannelBase + i, 0);
      ledcWrite(ledChannelBase + (8 - i), 0);
    }
    delay(500);
  }
}

void Round10(){
  delay(500);

  for(int i = 1; i<= 20; i++){
    ledcWrite(ledChannelBase + 1, i);
    ledcWrite(ledChannelBase + 2, i*2);
    ledcWrite(ledChannelBase + 3, i*3);
    ledcWrite(ledChannelBase + 4, i*4);
    ledcWrite(ledChannelBase + 5, i*3);
    ledcWrite(ledChannelBase + 6, i*2);
    ledcWrite(ledChannelBase + 7, i);
    delay(10);
  }

  for(int i = 1; i<= 255; i++){
    ledcWrite(ledChannelBase + 0, i);
    ledcWrite(ledChannelBase + 8, i);
   
    if(80 + i <= 255) ledcWrite(ledChannelBase + 4, 80 + i);
    if(60 + i <= 255){
      ledcWrite(ledChannelBase + 3, 60 + i);
      ledcWrite(ledChannelBase + 5, 60 + i);
    }
    if(40 + i <= 255){
      ledcWrite(ledChannelBase + 2, 40 + i);
      ledcWrite(ledChannelBase + 6, 40 + i);
    }
    if(20 + i <= 255){
      ledcWrite(ledChannelBase + 1, 20 + i);
      ledcWrite(ledChannelBase + 7, 20 + i);
    }

    delay(25);
  }
}

void Round11(){
  delay(500);
  for(int i = 0; i<4; i++){
    ledcWrite(ledChannelBase + i, 170);
    ledcWrite(ledChannelBase + 8 - i, 170);
    delay(200);
  }
}

void Round12(){
  delay(500);
  for(int i = 1; i <= 85; i++){
    for(int j = 0; j < 4; j++){
      ledcWrite(ledChannelBase + j, 170 + i);
      ledcWrite(ledChannelBase + 8 - j, 170 + i);
    }
    ledcWrite(ledChannelBase + 4, 255 - i*3);
    delay(100);
  }
}

void Round13(){
  delay(2000);
  for(int i = 1; i <= 127; i++){
    for(int j = 0; j < 4; j++){
      ledcWrite(ledChannelBase + j,255 - i);
      ledcWrite(ledChannelBase + 8 - j,255 - i);
    }
    ledcWrite(ledChannelBase + 4, 10 + i);
    delay(70);
  }
}

void Round14(){
  delay(500);
  for(int i=0; i<9; i+=2) ledcWrite(ledChannelBase + i, 255);
  delay(700);
  for(int i=1; i<9; i+=2) ledcWrite(ledChannelBase + i, 255);
  delay(700);
}

void Round15(){
  for(int i=255; i>=0; i--){
    ledcWrite(ledChannelBase + 4, i);
    delay(50);
  }
  ledcWrite(ledChannelBase + 4, 255);
  delay(500);
  ALL_ON(500);
}

void Round16(){
  for(int i = 8; i > 0; i--){
    for(int j = 8; j >= i; j--){
      ledcWrite(ledChannelBase + j, 10*(j-i+1));
    }
    delay(50);
  }
  int strength;
  for(int i = 1; i <= 245; i++){
    for(int j = 1; j < 9; j++){
      strength = j*10 + i;
      if(strength < 256) ledcWrite(ledChannelBase + j, strength);
    }
    delay(20);
  }  
}

void Round17(){
  delay(2000);
  for(int i = 1; i < 9; i++){
    ledcWrite(ledChannelBase + i, 0);
    delay(100);
  }
  delay(1000);
  for(int i = 9; i >= 0; i--){
    ledcWrite(ledChannelBase + i, 255);
    delay(20);
  }
  delay(10000);
}

void loop() {
  Round1();
  Round2();
  Round3();
  Round4();
  Round5();
  Round6();
  Round7();
  Round8();
  Round9();
  Round10();
  Round11();
  Round12();
  Round13();
  Round14();
  Round15();
  Round16();
  Round17();
}