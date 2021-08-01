#include "config.h"

#define LONG_PRESS_TIME 400
#define HOLD_TIME 1500

#define TOLERANCE 90
#define NUM_KEYS 2
int16_t keys[5] = {1960, 480, 890, 680, 1160};

int16_t keyHold[NUM_KEYS] = {-1, -1};
int16_t keyPrev[NUM_KEYS] = {-1, -1};
long keyChangeTime[NUM_KEYS] = {0, 0};
uint8_t keyLongPressSent[NUM_KEYS] = {0, 0};

void steeringKeysBegin()
{
  pinMode(KEY0_PIN, INPUT);
  pinMode(KEY1_PIN, INPUT);
}

int16_t getButton(int16_t raw)
{
  for (int16_t i = 0; i < 5; i++)
  {
    int16_t val = keys[i];
    if (abs(raw - val) < TOLERANCE)
      return i;
  }
  return -1;
}

int16_t steeringKeysCheckHold()
{
  if (keyHold[0] != -1)
    return keyHold[0];
  if (keyHold[1] != -1)
    return keyHold[1] + 5;
  return -1;
}

int16_t steeringKeysCheckPress()
{
  if (keyPrev[0] != -1)
    return keyPrev[0];
  if (keyPrev[1] != -1)
    return keyPrev[1] + 5;
  return -1;
}

void steeringKeysLoop()
{
  uint16_t raw0 = analogRead(KEY0_PIN);
  uint16_t raw1 = analogRead(KEY1_PIN);
  int16_t key[2];
  key[0] = getButton(raw0);
  key[1] = getButton(raw1);

  for (int i = 0; i < NUM_KEYS; i++)
  {
    if (keyPrev[i] != key[i])
    {
      //Serial.print("Change: "); Serial.print(keyPrev[i]); Serial.print("->");Serial.print(key[i]);Serial.println("");
      if (keyHold[i] == -1 && keyPrev[i] != -1 && keyLongPressSent[i] == 0)
      {
        //Serial.print("Short Press: "); Serial.println(keyPrev[i]);
        steeringKeysCallback(keyPrev[i] + i * 5, SHORT_PRESS);
      }
      keyLongPressSent[i] = 0;
      keyChangeTime[i] = millis();
    }

    if (key[i] != -1 && (millis() - keyChangeTime[i]) > LONG_PRESS_TIME)
    {
      if (keyLongPressSent[i] == 0)
      {
        keyLongPressSent[i] = 1;
        //Serial.print("Long Press: "); Serial.println(keyPrev[i]);

        steeringKeysCallback(keyPrev[i] + i * 5, LONG_PRESS);
      }
    }

    if (key[i] != -1 && (millis() - keyChangeTime[i]) > HOLD_TIME)
    {
      if (keyHold[i] != key[i])
      {
        //Serial.print("Hold started: "); Serial.println(key[i]);
        keyHold[i] = key[i];
        
        steeringKeysCallback(keyHold[i] + i * 5, HOLD_START);

        
      }
    }
    else
    {
      if (keyHold[i] != -1)
      {
        //Serial.print("Hold stopped: "); Serial.println(keyHold[i]);
        steeringKeysCallback(keyHold[i] + i * 5, HOLD_START);
        keyHold[i] = -1;
      }
    }
    keyPrev[i] = key[i];
  }
}
