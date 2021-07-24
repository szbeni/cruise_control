

void debugPrint(String str)
{
  #ifdef CC_DEBUG_ENABLE 
    Serial.println(str);
  #endif
}

void cruiseControlBegin()
{
  pinMode(BRAKE_PEDAL_PIN, INPUT);
  cc.screenMode = SCREEN_MODE_SPEED;
  cc.mode = MODE_NOT_INITIALIZED;
  cc.throttleInPrev = -1;
  cc.manualThrottle = 0;
  cc.currentSpeed = -1;
  cc.targetSpeed = 0;
  cc.throttleOut = 0;  
  cc.iTerm = 0.01f;
}

void steeringKeysCallback(int16_t key, uint8_t pressType)
{
  if(key == KEY_DISPLAY && pressType == SHORT_PRESS)
  {
    Serial.println(cc.screenMode);
    cc.screenMode += 1;
    
    if (cc.screenMode >= SCREEN_MODE_NUM)
    {
      cc.screenMode = 0;
    }
  }

  if(cc.mode == MODE_NORMAL)
  {
    if(key == KEY_UP && pressType == LONG_PRESS)
    {
      cc.mode = MODE_SPEED;
    }
  }

  if(cc.mode == MODE_THROTTLE)
  {
    float change = 0.02f;
    if(pressType == LONG_PRESS) change *= 2.0;
    if (key == KEY_UP) cc.manualThrottle = changeThrottle(cc.manualThrottle, change);
    if(key == KEY_DOWN) cc.manualThrottle = changeThrottle(cc.manualThrottle, -change);
  }

 
  if(cc.mode == MODE_SPEED)
  {
    float change = 1.0f;
    if(pressType == LONG_PRESS) change *= 2.0;
    if(key == KEY_UP) cc.targetSpeed = changeTargetSpeed(cc.targetSpeed, change);
    if(key == KEY_DOWN) cc.targetSpeed = changeTargetSpeed(cc.targetSpeed, -change);
    
    if(key == KEY_MODE) cc.iTerm += 0.001f;
    if(key == KEY_UNIT) 
    {
      cc.iTerm -= 0.001f;
      if (cc.iTerm < 0) cc.iTerm = 0;
    }

  }  
  Serial.print("Press: "); Serial.print(key); Serial.println(pressType);
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
    return;
    
  if (cc.mode == MODE_INITIALIZED)
  {
    cc.mode = MODE_NORMAL;
  }


  cc.brakeState = digitalRead(BRAKE_PEDAL_PIN);

  if (cc.brakeState == HIGH)
  {
    if(cc.mode != MODE_ERROR)
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
  
  
  if (cc.mode == MODE_NORMAL)
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
  }
  else if (cc.mode == MODE_ERROR)
  {
    cc.throttleOut = 0;
  }
  
  pedal.setThrottle(cc.throttleOut);
  cc.throttleInPrev = cc.throttleIn;
}



float speedController(float target, float current, float dT)
{
  float error;
  error = (int)target - current;
  if(error > 5.0f) error = 5.0f;
  else if(error < -5.0f) error = -5.0f;
  
  cc.iState += error * cc.iTerm * dT;
  if(cc.iState < THROTTLE_MIN) cc.iState = 0.0f;
  else if(cc.iState > THROTTLE_MAX) cc.iState = 1.0f;
  return cc.iState; 

  
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
