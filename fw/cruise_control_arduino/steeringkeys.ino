#include "config.h"

#define LONG_PRESS_TIME 400
#define HOLD_TIME 1500

#define TOLERANCE 90
int16_t keys[5] = {1960, 480, 890, 680, 1160};
int16_t key0Hold = -1;
int16_t key1Hold = -1;

int16_t key0Prev = -1;
int16_t key1Prev = -1;
long key0ChangeTime = 0;
long key1ChangeTime = 0;

void steeringKeysBegin()
{
  pinMode(KEY0_PIN, INPUT);
  pinMode(KEY1_PIN, INPUT);
}

int16_t getButton(int16_t raw)
{
  for (int16_t i=0;i<5;i++)
  {
    int16_t val = keys[i];
    if (abs(raw - val) < TOLERANCE)
      return i;  
  }
  return -1;
}



int16_t steeringKeysCheckHold()
{
    if (key0Hold != -1) return key0Hold;
    if (key1Hold != -1) return key1Hold+5;
    return -1;

}

int16_t steeringKeysCheckPress()
{
  if (key0Prev != -1) return key0Prev;
  if (key1Prev != -1) return key1Prev + 5;
  return -1;
}

void steeringKeysLoop()
{
  uint16_t raw0 = analogRead(KEY0_PIN);
  uint16_t raw1 = analogRead(KEY1_PIN);

  int16_t key0 = getButton(raw0); 
  int16_t key1 = getButton(raw1);
  uint8_t pressType;

  if (key0Prev != key0)
  {
    //Serial.print("Change: "); Serial.print(key0Prev); Serial.print("->");Serial.print(key0);Serial.println("");
    if (key0Hold == -1 && key0Prev != -1){
      pressType = SHORT_PRESS;
      if((millis() - key0ChangeTime) > LONG_PRESS_TIME)
        pressType = LONG_PRESS;
      steeringKeysCallback(key0Prev, pressType);
    }
    key0ChangeTime = millis();
  }
  
  if (key0 != -1 && (millis() - key0ChangeTime) > HOLD_TIME)
  {
    if(key0Hold != key0)
    {
      //Serial.print("Hold started: "); Serial.println(key0);
      key0Hold = key0;

     
    }
  }
  else
  {
    if(key0Hold != -1)
    {
     //Serial.print("Hold stopped: "); Serial.println(key0Hold);
      key0Hold = -1;    
    }
  }
  
  

  if (key1Prev != key1)
  {
    //Serial.print("Change: "); Serial.print(key1Prev); Serial.print("->");Serial.print(key1);Serial.println("");
    if (key1Hold == -1 && key1Prev != -1){
      pressType = SHORT_PRESS;
      if((millis() - key1ChangeTime) > LONG_PRESS_TIME)
        pressType = LONG_PRESS;
      steeringKeysCallback(key1Prev + 5, pressType);
    }
    key1ChangeTime = millis();

  }
  
  if (key1 != -1 && (millis() - key1ChangeTime) > HOLD_TIME)
  {
    if(key1Hold != key1)
    {
      //Serial.print("Hold started: "); Serial.println(key1);
      key1Hold = key1;   
    }
  }
  else
  {
    if(key1Hold != -1)
    {
      //Serial.print("Hold stopped: "); Serial.println(key1Hold);
      key1Hold = -1;    
    }
  }

  key0Prev = key0;
  key1Prev = key1;

}
