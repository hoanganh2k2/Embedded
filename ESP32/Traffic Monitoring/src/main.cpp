#include <Arduino.h>
#include <TM1637Display.h>
#include <AudioFileSourceFS.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include <SPIFFS.h>

#define TRIG_PIN 32
#define ECHO_PIN 35
#define CLK 23
#define DIO 21
#define RED 5
#define GREEN 19
#define YELLOW 18

TM1637Display display(CLK, DIO);
hw_timer_t * timer = NULL;

#define RED_TIME 20
#define GREEN_TIME 15
#define YELLOW_TIME 3

int status = 1; // 0: red, 1: green, 2: yellow
int timeNow = 14;
long duration;
float distance;

AudioGeneratorMP3 *mp3;
AudioFileSourceFS *warningRedAudio; 
AudioFileSourceFS *greenAudio; 
AudioFileSourceFS *detectAudio; 
AudioOutputI2S *out;

bool playedGreen = false;
bool playedWarning = false;
bool playedDetect = false;

unsigned long lastMeasure = 0;

void onLED(int color){
  switch (color)
  {
  case 0:
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    digitalWrite(YELLOW, LOW);
    break;
  case 1:
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
    digitalWrite(YELLOW, LOW);
    break;
  case 2:
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(YELLOW, HIGH);
    break;
  default:
    break;
  }
}

void IRAM_ATTR interrup(){
  if(timeNow == 0){
    switch (status)
    {
    case 0:
      status++;
      timeNow = GREEN_TIME;
      onLED(1);
      break;
    case 1: 
      status++; 
      timeNow = YELLOW_TIME; 
      onLED(2);
      break;
    case 2: 
      status = 0; 
      timeNow = RED_TIME; 
      onLED(0);
      break;
    default:break;
    }
  } else timeNow--;

  int minutes = timeNow / 60;
  int second = timeNow % 60;
  int time = minutes * 100 + second;
  display.showNumberDecEx(time, 0b01000000, true);
}

void setup_audio(){
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ Không thể mount SPIFFS.");
    return;
  }
  detectAudio = new AudioFileSourceFS(SPIFFS, "/detect.mp3");
  warningRedAudio = new AudioFileSourceFS(SPIFFS, "/warningRed.mp3");
  greenAudio = new AudioFileSourceFS(SPIFFS, "/green.mp3");

  out = new AudioOutputI2S();

  out->SetPinout(26, 25, 22);
  out->SetGain(0.6);

  mp3 = new AudioGeneratorMP3();
}

void setup() {
  Serial.begin(115200);
  display.setBrightness(0x02); // Điều chỉnh Độ sáng

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(GREEN, HIGH);

  timer = timerBegin(0, 80, true); // Bắt đầu timer 0, chia clock /80 → 1 tick = 1 µs
  timerAttachInterrupt(timer, &interrup, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);

  setup_audio();
}

void redLightAction(){
  long timeNow = millis();
  if(timeNow - lastMeasure > 200){
    lastMeasure = timeNow;
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
  
    duration = pulseIn(ECHO_PIN, HIGH, 30000);
    distance = duration * 0.034 / 2;
  
    Serial.println(distance);
  
    if(distance < 5 && !playedDetect){
        mp3 = new AudioGeneratorMP3();
        detectAudio = new AudioFileSourceFS(SPIFFS, "/detect.mp3");
        detectAudio->seek(0, SEEK_SET);
        mp3->begin(detectAudio, out);
        playedDetect = true;
    }
  }

}

void loop() {
  if(mp3 && mp3->isRunning()){
    if(!mp3->loop()){
      mp3->stop();
      delete mp3;
      mp3 = nullptr;
      playedDetect = false;
      playedGreen = false;
      playedWarning = false;
    }
  }

  if(status == 0) redLightAction();
  else{
    if(status == 1 && timeNow == 5 && !playedWarning){
        mp3 = new AudioGeneratorMP3();
        warningRedAudio = new AudioFileSourceFS(SPIFFS, "/warningRed.mp3");
        warningRedAudio->seek(0, SEEK_SET);
        mp3->begin(warningRedAudio, out);
        playedWarning = true;
    }
    if(status == 1 && timeNow == GREEN_TIME && !playedGreen){
        mp3 = new AudioGeneratorMP3();
        greenAudio = new AudioFileSourceFS(SPIFFS, "/green.mp3");
        greenAudio->seek(0, SEEK_SET);
        mp3->begin(greenAudio, out);
        playedGreen = true;
    }
  }
}
