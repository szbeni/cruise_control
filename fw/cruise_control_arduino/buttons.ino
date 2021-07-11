
#define SHORT_PRESS_TIME 50
#define LONG_PRESS_TIME 300
#define HOLD_TIME 1500



int lastSent = 0;
int currentState[BTN_NUM]  = {HIGH,HIGH};
int lastState[BTN_NUM]  = {HIGH,HIGH};
bool holdStateOn[BTN_NUM] = {false, false};
unsigned long pressedTime[BTN_NUM] = {0,0};
unsigned long releasedTime[BTN_NUM]  = {0,0};


void startButtons() {
  pinMode(BTN_UP_PIN, INPUT_PULLUP);
  pinMode(BTN_DOWN_PIN, INPUT_PULLUP);
}

void buttonsLoop() {
  // read the state of the switch/button:
  currentState[0] = digitalRead(BTN_UP_PIN);
  currentState[1] = digitalRead(BTN_DOWN_PIN);
  
  if(millis()-lastSent > 10)
  {
    lastSent = millis();
    for(int i=0;i<BTN_NUM;i++)
    {
      if(holdStateOn[i])
        buttonHold(i);
    }
      
    //Serial.printf("Button %d, %d\n", currentState[0],currentState[1]);  
  }
  
  for(int i=0;i<BTN_NUM;i++)
  {
    if(lastState[i] == HIGH && currentState[i] == LOW)
    {
      pressedTime[i] = millis();
    }
    else if(lastState[i] == LOW && currentState[i] == HIGH)
    {
      releasedTime[i] = millis();  
      holdStateOn[i] = false;
      long pressDuration = releasedTime[i] - pressedTime[i];
      if( pressDuration > LONG_PRESS_TIME &&  pressDuration < HOLD_TIME)
      {
        buttonLongPress(i);
        Serial.printf("Button %d long press\n",i);
      }
      else if(pressDuration > SHORT_PRESS_TIME &&  pressDuration < HOLD_TIME)
      {
        buttonShortPress(i);
        Serial.printf("Button %d short press\n", i);
      }
      else if(pressDuration > HOLD_TIME)
      {
        Serial.printf("Button %d hold release\n", i);
      }
      else
      {
        //debounce
      }
    }
    else if(lastState[i] == LOW && currentState[i] == LOW)
    {
      if((millis() - pressedTime[i]) > HOLD_TIME)
      {
        if(holdStateOn[i] == false)
        {
          holdStateOn[i] = true;
         
          Serial.printf("Hold\n");          
        }
      }
    }
     
    lastState[i] = currentState[i];
  }
}
