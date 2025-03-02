#include <Wire.h>

#define LED A0
#define ARDUINO_I2C_ADDR 6

void setup()
{
  Wire.begin(ARDUINO_I2C_ADDR); 
  Wire.onReceive(receiveEvent); 
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
}
 
void loop()
{
}
 
void receiveEvent() 
{
  while(Wire.available()) 
  {
    char c = Wire.read();
    
    if(c == 'H')
    {
      digitalWrite(LED,HIGH); 
    }
    else if(c == 'L') 
    {
      digitalWrite(LED,LOW);
    }
  }
}