

void debugPrint(String str)
{
  #ifdef CC_DEBUG_ENABLE 
    Serial.println(str);
  #endif
}

void cruiseControlBegin()
{
  pinMode(ANALOG_MUX_PIN, OUTPUT);
  pinMode(BRAKE_PEDAL_PIN, INPUT);
  digitalWrite(ANALOG_MUX_PIN, LOW);
  cc.screenMode = SCREEN_MODE_SPEED;
  cc.mode = MODE_NOT_INITIALIZED;
  cc.throttleInPrev = -1;
  cc.manualThrottle = 0;
  cc.currentSpeed = -1;
  cc.targetSpeed = 0;
  cc.throttleOut = 0;  
  cc.unit = UNIT_MPH;
  cc.pTerm = 0.030f;
  cc.iTerm = 0.015f;
  cc.dTerm = 0.001f;
  cc.iState = 0.0f;
  cc.clearDTCFlag = false;
  cc.writePedalToEepromFlag = false;
  
}


String pressTypeStr[4] = {"Short", "Long", "Hold Start", "Hold Stop" };
void steeringKeysCallback(int16_t key, uint8_t pressType)
{
  if(cc.mode == MODE_LOCKED)
  {
    if(key == KEY_UP && pressType == SHORT_PRESS)
    {
      //if (cc.brakeState == HIGH &&  cc.throttleIn > 0.5)
      if (cc.brakeState == HIGH)
      {
        cc.mode = MODE_NORMAL;
      }
    }
  }
  if(key == KEY_DISPLAY && pressType == SHORT_PRESS)
  {
    Serial.println(cc.screenMode);
    cc.screenMode += 1;
    
    if (cc.screenMode >= SCREEN_LOOP_NUM)
    {
      cc.screenMode = 0;
    }
  }
  
  if(key == KEY_DISPLAY && pressType == LONG_PRESS)
  {
    Serial.println("Clear DTC requsted");
    cc.clearDTCFlag = true;
  }


  if(cc.mode == MODE_THROTTLE)
  {
    float change = 0.02f;
    if(pressType == LONG_PRESS) change *= 2.0;
    if (key == KEY_UP) cc.manualThrottle = changeThrottle(cc.manualThrottle, change);
    if(key == KEY_DOWN) cc.manualThrottle = changeThrottle(cc.manualThrottle, -change);
  }

  if(cc.screenMode == SCREEN_MODE_SPEED)
  {
    if(key == KEY_UNIT) cc.unit = !cc.unit;
    if(cc.mode == MODE_SPEED)
    {
      float change = 1.0f;
      if(pressType == LONG_PRESS) change *= 2.0;
      if(key == KEY_UP) cc.targetSpeed = changeTargetSpeed(cc.targetSpeed, change);
      if(key == KEY_DOWN) cc.targetSpeed = changeTargetSpeed(cc.targetSpeed, -change);
      if(key == KEY_MODE) cc.mode = MODE_NORMAL; 
    }
  }

  if(cc.screenMode == SCREEN_MODE_SETTINGS)
  {
    float change = 1.0f;
    if(pressType == LONG_PRESS) change *= 2.0;
    if(key == KEY_UP) cc.pTerm = changeValue(cc.pTerm, 0.001f, 0, 100);
    if(key == KEY_DOWN) cc.pTerm = changeValue(cc.pTerm, -0.001f, 0, 100);
    if(key == KEY_MODE) cc.iTerm = changeValue(cc.iTerm, 0.001f, 0, 100);
    if(key == KEY_UNIT) cc.iTerm = changeValue(cc.iTerm, -0.001f, 0, 100);
    if(key == KEY_SPARE1) cc.dTerm = changeValue(cc.dTerm, 0.001f, 0, 100);
    if(key == KEY_SPARE2) cc.dTerm = changeValue(cc.dTerm, -0.001f, 0, 100);
    


    
  }

  if(cc.screenMode == SCREEN_MODE_DEBUG1)
  {
    if(key == KEY_UP) {
      cc.writePedalToEepromFlag = true;
    }
    


    
  }




  if(cc.mode == MODE_NORMAL)
  {
    if(key == KEY_UP && pressType == LONG_PRESS)
    {
      if(cc.currentSpeed > SPEED_MIN)
      {
        cc.targetSpeed = cc.currentSpeed;
        cc.mode = MODE_SPEED;  
      }
    }
  }

  
//  Serial.print(pressTypeStr[pressType]);  
//  Serial.print(" pressed: "); 
//  Serial.println(key); 
  
}

void holdKeyCheck()
{
  int16_t key = steeringKeysCheckHold();
  if(cc.mode == MODE_THROTTLE)
  {
    if (key == KEY_UP) cc.manualThrottle = changeThrottle(cc.manualThrottle, 0.01f);
    if(key == KEY_DOWN) cc.manualThrottle = changeThrottle(cc.manualThrottle, -0.01f);
  }

  if(cc.mode == MODE_SPEED)
  {
    if (key == KEY_UP) cc.targetSpeed = changeTargetSpeed(cc.targetSpeed, 0.1f);
    if(key == KEY_DOWN) cc.targetSpeed = changeTargetSpeed(cc.targetSpeed, -0.1f);
  }
  
}




void cruiseControlLoop()
{ 
  unsigned long now = millis();
  cc.dT = 1.0f / (float)(now - cc.lastTick);
  cc.lastTick = now;

  if (cc.mode == MODE_NOT_INITIALIZED)
  {
    pedal.setThrottle(0);
    return;
  }
    
    
  if (cc.mode == MODE_INITIALIZED)
  {
    cc.mode = MODE_LOCKED;
  }

  cc.brakeState = digitalRead(BRAKE_PEDAL_PIN);

  // When brake is pressed to to normal state
  if (cc.brakeState == HIGH)
  {
    if(cc.mode != MODE_ERROR && cc.mode != MODE_LOCKED )
    {
      cc.mode = MODE_NORMAL;  
    }
  }
  
  steeringKeysLoop();
  holdKeyCheck();
  cc.throttleIn = pedal.getThrottle();
  if(cc.throttleIn < 0.0f)
  {
    cc.mode = MODE_ERROR;
    cc.throttleOut = 0;
  }
  else
  {
    //get out of error mode if pedal is detected
//    if (cc.mode == MODE_ERROR)
//    {
//      cc.mode = MODE_NORMAL;
//    }
  }

  if (cc.mode == MODE_LOCKED)
  {
    cc.throttleOut = 0;
  }
  else if (cc.mode == MODE_NORMAL)
  {
    cc.throttleOut = cc.throttleIn;
    //Hold last value
    cc.manualThrottle = cc.throttleIn;
    //Change the speed controller to the current throttle
    cc.iState = cc.throttleIn;
  }
  else if (cc.mode == MODE_THROTTLE)
  {
    cc.throttleOut = cc.manualThrottle;
    if ((cc.throttleIn - 0.02f) > cc.throttleInPrev)
    {
      cc.mode = MODE_NORMAL;
    }

  }
  else if (cc.mode == MODE_SPEED)
  {
    
    cc.throttleOut = speedController(cc.targetSpeed, cc.currentSpeed, cc.dT);
    if ((cc.throttleIn - 0.02f) > cc.throttleInPrev)
    {
      cc.mode = MODE_NORMAL;
    }
    if(cc.rpm > 3200.0)
    {
      cc.mode  = MODE_NORMAL;
    }
    // float rpmDiff = cc.rpm - cc.rpmPrev;
    // if(cc.rpm > 2000 && rpmDiff > 100)
    // {
    //   cc.mode  = MODE_NORMAL;
    // }
  }
  else if (cc.mode == MODE_ERROR)
  {
    cc.throttleOut = 0;
  }


  // Analog MUX, select DAC output when in speed mode, otherwise use pedal from car
  if ((cc.mode == MODE_LOCKED && cc.rpm > 600.0) || (cc.mode == MODE_SPEED))
  {
    digitalWrite(ANALOG_MUX_PIN, HIGH);
  }
  else
  {
    digitalWrite(ANALOG_MUX_PIN, LOW);
  }
  
  
  bool writeEEPROM = false;
  //Do it only once when user requested
  if (cc.writePedalToEepromFlag)
  {
    Serial.println("Write to EEPROM.");
    cc.writePedalToEepromFlag = false;
    writeEEPROM = true;
  }
  pedal.setThrottle(cc.throttleOut, writeEEPROM);
  cc.throttleInPrev = cc.throttleIn;
  cc.rpmPrev = cc.rpm;
}



float speedController(float target, float current, float dT)
{
  float error;
  error = target - current;
  
  if(error > 10.0f) error = 10.0f;
  else if(error < -10.0f) error = -10.0f;
 
  cc.error = error;


  if(error < 0)
  {
    //make integrator term harher when we are speeding
    cc.iState += -error * error * cc.iTerm * dT;
    
  }
  else
  {
    cc.iState += error * cc.iTerm * dT;
  
  }
  
  cc.speedOut = error * cc.pTerm + cc.iState + (cc.error - cc.prevError) * cc.dTerm;
  cc.prevError = error;
  
  //down hill when we suddenly starting to accelerate
  //if (error < -2) cc.iState = 0.0f;
    
  if(cc.iState < THROTTLE_MIN) cc.iState = 0.0f;
  else if(cc.iState > THROTTLE_MAX) cc.iState = 1.0f;

  if(cc.speedOut < THROTTLE_MIN) cc.speedOut = 0.0f;
  else if(cc.speedOut > THROTTLE_MAX) cc.speedOut = 1.0f;
    
  return cc.speedOut;
}


float changeValue(float currentValue, float change, float min, float max)
{
  currentValue += change;
  if (currentValue > max)
  {
    currentValue = max;
  }
  else if(currentValue < min)
  {
   currentValue = min;
  }
  return currentValue;
}


float changeThrottle(float currentValue, float change)
{
  currentValue += change;
  if (currentValue > THROTTLE_MAX)
  {
    currentValue = THROTTLE_MAX;
  }
  else if(currentValue < THROTTLE_MIN)
  {
   currentValue = THROTTLE_MIN;
  }
  return currentValue;
}


float changeTargetSpeed(float currentValue, float change)
{
  currentValue += change;
  if (currentValue > SPEED_MAX)
  {
    currentValue = SPEED_MAX;
  }
  else if(currentValue < SPEED_MIN)
  {
   currentValue = SPEED_MIN;
  }
  return currentValue;
}
  
void cruiseControlSetMode(uint8_t mode)
{
  if(cc.mode != mode)
  {
    cc.mode = mode;
    debugPrint("Mode changed: " + String(mode));
  }
}
