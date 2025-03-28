//blynk
#define BLYNK_TEMPLATE_ID "TMPL6VNpA5rSd"
#define BLYNK_TEMPLATE_NAME "System Home"
#define BLYNK_AUTH_TOKEN "woEtFYy7ScCUdkTAIJ0z2Y9MmgglJ6dS"
#define BLYNK_TEMPLATE_VERSION "0.1.0"

#define BLYNK_PRINT Serial
#define APP_DEBUG
#define USE_NODE_MCU_BOARD

#include <Arduino.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp32.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define TRIG_PIN 32
#define ECHO_PIN 35
#define SERVO_PIN 17
#define RELAY_PIN 15
#define LED_PIN 23
#define BUZZER_PIN 4
#define FAN_PIN 15
#define BUTTON_DOOR_PIN 18
#define BUTTON_HCSR04_PIN 19

#define BUZZER_PWM_CHANNEL 8
#define PWM_FREQ 2000     
#define PWM_RESOLUTION 8  

#define SERVO_PWM_CHANNEL 0 
#define SERVO_PWM_FREQ 50  
#define SERVO_PWM_RESOLUTION 16  

const byte ROWS = 4; 
const byte COLS = 3; 
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {33, 25, 26, 27}; 
byte colPins[COLS] = {14, 12, 13};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int countWrongPass = 0;

// lcd
String home_pass = "1905";
String LCD_string = "";

bool isDoorOpen = false;
bool isMenuOpen = false;
bool isChangPass = false;
bool isAuthenSuccess = false;
bool isLcdNotification = false;
bool isHCSR04Open = true;
bool isOpenInside = false;
bool isDisableKeypad = false;

const unsigned long lcdDelay = 3000;
unsigned long lcdOpenTime = 0;

// buzzer
enum BeepState {Idle, Short_Beep, Long_Beep, Pause};
BeepState beepState = Idle;
unsigned long beepStartTime = 0;
const unsigned long shortBeepDuration = 200;
const unsigned long longBeepDuration = 600;
const unsigned long pauseDuration = 100;
int beepCount = 0;

// hcsr04
long duration;
float distance;
unsigned long hcsr04Time = 0;
unsigned long timeDelayDoorButton = 0;
unsigned long timeDelayHcsr04Button = 0;

#define WIFI_SSID "SH3 P402"
#define WIFI_PASSWORD "studenthouse402"
WidgetLED LED_CONNECTION(V0);

void resetLCD(){
  lcd.clear();
  LCD_string = "";
  lcd.setCursor(0, 0);
  lcd.print("Enter Home Pass");
  lcd.setCursor(0, 1);
}

void startBeep(){
  beepState = Short_Beep;
  beepStartTime = millis();
  beepCount = 0;
}

void updateBuzzer(){
  unsigned long currentTime = millis();
  switch (beepState)
  {
    case Short_Beep:
      if(currentTime - beepStartTime > shortBeepDuration){
        ledcWrite(BUZZER_PWM_CHANNEL, 0); 
        beepState = Pause;
        beepStartTime = currentTime;
      }
      break;
    case Long_Beep:
      if(currentTime - beepStartTime > longBeepDuration){
        ledcWrite(BUZZER_PWM_CHANNEL, 0); 
        beepState = Pause;
        beepStartTime = currentTime;
      }
      break;
    case Pause:
      if(currentTime - beepStartTime > pauseDuration){
        beepCount++;
        if(beepCount < 4){
          ledcWrite(BUZZER_PWM_CHANNEL, 128);
          beepState = Short_Beep;
          beepStartTime = currentTime;
        }
        else if(beepCount < 6){
          ledcWrite(BUZZER_PWM_CHANNEL, 170);
          beepState = Long_Beep;
          beepStartTime = currentTime;
        }
        else if(beepCount < 9){
          ledcWrite(BUZZER_PWM_CHANNEL, 128);
          beepState = Short_Beep;
          beepStartTime = currentTime;
        }
        else if(beepCount < 11){
          ledcWrite(BUZZER_PWM_CHANNEL, 170);
          beepState = Long_Beep;
          beepStartTime = currentTime;
        }
        else beepState = Idle;
      }
      break;
    case Idle:
      break;
    default:
      break;
  }
}

int servoPulseWidth(int angle) {
  int minVal = 1638; // 0 độ với tần số 50 Hz
  int maxVal = 8192; // 180 độ
  return map(angle, 0, 180, minVal, maxVal);
}

void disableLCD(){
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Warning");
  lcd.setCursor(0, 1);
  lcd.print("Too Many Wrong!");
  isDisableKeypad = true;
}

void checkHomePass(){
  if(countWrongPass >= 3){
    if(!isDisableKeypad) disableLCD();
    return;
  }

  char key = keypad.getKey();
  if (key && !isLcdNotification) {
    if (key == '#') {
      if(isChangPass){
        if (LCD_string == home_pass) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Enter New Pass");
          lcd.setCursor(0, 1);
          LCD_string = "";
          isAuthenSuccess = true;
        } else if(isAuthenSuccess){
          isAuthenSuccess = false;
          isChangPass = false;
          home_pass = LCD_string;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Successfully!");
          lcdOpenTime = millis();
          isLcdNotification = true;
        }
        else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wrong Password!!");
          countWrongPass++;
          lcd.setCursor(0, 1);
          lcd.print(3 - countWrongPass);
          lcd.print(" attempts left");
          lcdOpenTime = millis();
          isLcdNotification = true;
          startBeep();
        }
      }
      else{
        if (LCD_string == home_pass) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Access Granted");
          lcdOpenTime = millis();
          isLcdNotification = true;
          ledcWrite(SERVO_PWM_CHANNEL, servoPulseWidth(90));
          if(isDisableKeypad){
            isDisableKeypad= false;
            resetLCD();
          }
          countWrongPass = 0;
          isDoorOpen = true; 
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wrong Password");
          countWrongPass++;
          lcd.setCursor(0, 1);
          lcd.print(3 - countWrongPass);
          lcd.print(" attempts left");
          lcdOpenTime = millis();
          isLcdNotification = true;
          startBeep();
        }
      } 
    } else if (key == '*'){
      if (!LCD_string.isEmpty()) {
        int length = LCD_string.length();
        LCD_string.remove(length - 1);
        lcd.setCursor(length - 1, 1);
        lcd.print(" ");
        lcd.setCursor(length - 1, 1);
      }
      else if(!isMenuOpen){
        lcd.setCursor(0, 0);
        lcd.print("1.Close The Door");
        lcd.setCursor(0, 1);
        lcd.print("2.Change Pass");
        isMenuOpen = true;
      }
      else{
        resetLCD();
        isMenuOpen = false;
      }
    } else if(isMenuOpen){
      if(key == '1'){
        ledcWrite(SERVO_PWM_CHANNEL, servoPulseWidth(0));
        isOpenInside = false;
        isDoorOpen = false; 
        resetLCD();
        isMenuOpen = false;
      }
      else if(key == '2'){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Pass Authen");
        lcd.setCursor(0, 1);
        isMenuOpen = false;
        isChangPass = true;
      }
    }
    else{
      LCD_string += key;
      lcd.print(key);
    }
  }
}

void closeByHCSR04(){
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  hcsr04Time = millis();

  duration = pulseIn(ECHO_PIN, HIGH, 25000); // Timeout 25ms (~4m)
  distance = duration * 0.034 / 2;

  if(distance <= 7){
    ledcWrite(SERVO_PWM_CHANNEL, servoPulseWidth(0));
    isOpenInside = false;
    isDoorOpen = false;
  }
}

void doorInterrup(){
  unsigned long currentTime = millis();
  if(currentTime - timeDelayDoorButton > 200 && !digitalRead(BUTTON_DOOR_PIN)){
    if(isDoorOpen){
      isDoorOpen = false;
      ledcWrite(SERVO_PWM_CHANNEL, servoPulseWidth(0));
      isOpenInside = false;
    }
    else {
      isDoorOpen = true;
      ledcWrite(SERVO_PWM_CHANNEL, servoPulseWidth(90));
      if(isDisableKeypad){
        isDisableKeypad = false;
      }
      countWrongPass = 0;
      isOpenInside = true;
    }
    timeDelayDoorButton = currentTime;
  }
}

void hcsr04Interrup(){
  unsigned long currentTime = millis();
  if(currentTime - timeDelayHcsr04Button > 200 && !digitalRead(BUTTON_HCSR04_PIN)){
    isHCSR04Open = !isHCSR04Open;
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    timeDelayHcsr04Button = currentTime;
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  pinMode(BUTTON_DOOR_PIN, INPUT_PULLUP);
  pinMode(BUTTON_HCSR04_PIN, INPUT_PULLUP);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  ledcSetup(BUZZER_PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(BUZZER_PIN, BUZZER_PWM_CHANNEL);

  ledcSetup(SERVO_PWM_CHANNEL, SERVO_PWM_FREQ, SERVO_PWM_RESOLUTION);
  ledcAttachPin(SERVO_PIN, SERVO_PWM_CHANNEL);

  ledcWrite(SERVO_PWM_CHANNEL, servoPulseWidth(0));

  lcd.init();
  lcd.backlight();
  resetLCD();

  digitalWrite(LED_PIN, HIGH);

  attachInterrupt(digitalPinToInterrupt(BUTTON_DOOR_PIN),doorInterrup, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_HCSR04_PIN),hcsr04Interrup, FALLING);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);
}

BLYNK_CONNECTED(){
  Blynk.syncAll();
}

BLYNK_WRITE(V1) {
  int state = param.asInt();
  digitalWrite(RELAY_PIN, state);
}

BLYNK_WRITE(V2) {
  int state = param.asInt();
  if(state == 0){ 
    ledcWrite(SERVO_PWM_CHANNEL, servoPulseWidth(0)); 
    isOpenInside = false;} 
  else {
    ledcWrite(SERVO_PWM_CHANNEL, servoPulseWidth(90)); 
    countWrongPass = 0; 
    if(isDisableKeypad){
      isDisableKeypad= false;
      resetLCD();
    }
  }
}

void loop() {
  if(isHCSR04Open && isDoorOpen && (millis() - hcsr04Time > 200) && !isOpenInside) closeByHCSR04();
  checkHomePass();
  if (isLcdNotification && millis() - lcdOpenTime >= lcdDelay) {
    if(countWrongPass < 3)resetLCD();
    isLcdNotification = false;
  }
  updateBuzzer();
  Blynk.run();
}