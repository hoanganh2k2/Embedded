#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define TRIG_PIN 18
#define ECHO_PIN 19
#define RED_PIN 12
#define BLUE_PIN 13
#define BUZZER_PIN 14

#define PWM_CHANNEL 0     
#define PWM_FREQ 2000     
#define PWM_RESOLUTION 8  

float prevDistance = -1;  
bool status = true;

void setup() {
  Serial.begin(115200); 

  lcd.init();
  lcd.backlight();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RED_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);

  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(BUZZER_PIN, PWM_CHANNEL);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  lcd.setCursor(0, 1);
  lcd.print("Safe Distance");
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = (duration > 0) ? (duration * 0.0343) / 2 : prevDistance;

  if (distance != prevDistance) { 
    prevDistance = distance; 

    lcd.setCursor(10, 0);
    lcd.print("          ");  
    lcd.setCursor(10, 0);
    lcd.print(distance, 1);  
    lcd.print("cm");

    if (distance < 5) {
      if(status){
        lcd.setCursor(0, 1);
        lcd.print("                  "); 
        lcd.setCursor(0, 1);
        lcd.print("Too Close!");
        status = !status;
      }
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(BLUE_PIN, LOW);
      ledcWrite(PWM_CHANNEL, 128);  
    } else {
      if(!status){
        lcd.setCursor(0, 1);
        lcd.print("Safe Distance");
        status = !status;
      }
      digitalWrite(BLUE_PIN, HIGH);
      digitalWrite(RED_PIN, LOW);
      ledcWrite(PWM_CHANNEL, 0);
    }
  }

  delay(500);
}
