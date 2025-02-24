#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
byte va[] = {
  B00000,
  B01100,
  B10010,
  B10100,
  B10101,
  B10010,
  B01101
};
void setup() {
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, va);
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Youtube Channel");
  delay(3000);
  lcd.clear();

  lcd.setCursor(1, 1);
  lcd.print("M");
  lcd.setCursor(2, 1);
  lcd.write(byte(0));
  lcd.setCursor(3, 1);
  lcd.print("E");
  lcd.setCursor(5, 1);
  lcd.print("Automation");
  delay(3000);
  lcd.clear();
  delay(3000);
  
  lcd.setCursor(0, 0);
  lcd.print("Youtube Channel");
  lcd.setCursor(1, 1);
  lcd.print("M");
  lcd.setCursor(2, 1);
  lcd.write(byte(0));
  lcd.setCursor(3, 1);
  lcd.print("E");
  lcd.setCursor(5, 1);
  lcd.print("Automation");
  delay(3000);
  lcd.clear();
  delay(3000);
}