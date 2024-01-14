HardwareSerial Serial2(PA3, PA2); //RX - PA3, TX - PA2

void obd2Begin()
{
  Serial2.begin(38400); 
  myELM327.begin(Serial2, false, 2000);  
}

int lastReconnectTime = 0;
void obd2Loop()
{
  
  if (myELM327.status != ELM_SUCCESS)
  {
    if (millis() - lastReconnectTime > 500)
    {
      lastReconnectTime = millis();
      myELM327.initializeELM();
    }
  }
}

float obd2GetSpeed(){ 
  if(myELM327.status == ELM_SUCCESS)
  {
    float kph = myELM327.kph();     
    if (myELM327.status == ELM_SUCCESS)
    {
      return kph;
    }
  }
  return -1;
}

float obd2GetRPM(){ 
  if(myELM327.status == ELM_SUCCESS)
  {
    float rpm = myELM327.rpm();     
    if (myELM327.status == ELM_SUCCESS)
    {
      return rpm;
    }
  }
  return -1;
}

uint8_t obd2ClearDTC(){ 
  if(myELM327.status  == ELM_SUCCESS)
  {
    myELM327.clearDTC();     
    if (myELM327.status == ELM_SUCCESS)
    {
      return 1;
    }
  }
  return 0;
}
