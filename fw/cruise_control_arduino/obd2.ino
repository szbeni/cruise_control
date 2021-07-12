void obd2Begin()
{
  Serial1.begin(38400); //TX-PA9/RX-PA10   connected to ELM327
  myELM327.begin(Serial1, true, 2000);  
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
  return 0.0;
}
