
void debugScreen2()
{
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner

  display.print("a: ");
  display.println(cc.currentSpeed);

  display.print("t: ");
  display.println(cc.targetSpeed);

  display.print("e: ");
  display.println(cc.error);

  display.print("o: ");
  display.println(cc.speedOut);

  //display.print("iS: ");
  //display.println(cc.iState);

  //display.print("P: ");
  //display.println(cc.pTerm * 1000.0f);

  //display.print("I: ");
  //display.println(cc.iTerm * 1000.0f);
}

void settingsScreen()
{
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);      
  display.print("P: ");
  display.println(cc.pTerm * 1000.0f);

  display.print("I: ");
  display.println(cc.iTerm * 1000.0f);

  display.print("D: ");
  display.println(cc.dTerm * 1000.0f);

}

void debugScreen1()
{

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner

  display.print("Mode:   ");
  display.println(modeName[cc.mode]);
  
  display.print("In: ");
  display.print(cc.throttleIn);
  
  display.print("    Out:");
  display.println(cc.throttleOut);
  
  display.print("Man: ");
  display.print(cc.manualThrottle);

  display.print("    Brk:");
  display.println(cc.brakeState);

  display.print("RPM: ");
  display.print(cc.rpm);

  display.print(" Key: ");
  display.println(steeringKeysCheckPress());
  display.println(cntr);
  
  display.print("speedtg: ");
  display.println(cc.targetSpeed);
  
  display.print("iTerm: ");
  display.println(cc.iTerm * 1000.0f);
  
  display.print("iState: ");
  display.println(cc.iState);
  
  //display.print("Speed: ");
}


void speedScreen()
{
  char buf[16];
  bool mph = (cc.unit == UNIT_MPH);
  float scalar = 1.0f;
  if (mph) scalar = 0.621f;
   
  display.setTextSize(4);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);
  sprintf(buf, "%3d", (int)(cc.targetSpeed*scalar));
  if (cc.mode == MODE_LOCKED)
  {
    display.setCursor(20,16);
    display.print("LOCK");
    display.drawRect(0, 0, 128, 64, SSD1306_WHITE);

  }
  else
  {
    display.print(buf);
    display.setCursor(95,22);
    display.setTextSize(1);
    if (mph)
      display.print("mph");
    else
      display.print("km/h");

    display.setTextSize(2);
    display.setCursor(40,34);
    sprintf(buf, "%3d", (int)(cc.currentSpeed * scalar));
    display.print(buf);
    
    display.setTextSize(1);
    display.setCursor(0,56);
    display.print("Mode: ");
    display.print(modeName[cc.mode]);
    if(cc.mode == MODE_SPEED)
    {
      display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
    }
  }
}



uint8_t screenBusy = 0;


void screenLoop(uint8_t screenMode)
{
  if(screenBusy == 0)
  {
     display.clearDisplay();

     switch(screenMode)
     {        
      case SCREEN_MODE_SPEED:
        speedScreen();
        break;
      
      case SCREEN_MODE_SETTINGS:
        settingsScreen();
        break;

      case SCREEN_MODE_DEBUG2:
        debugScreen2();
        break;
      
      case SCREEN_MODE_DEBUG1:
        debugScreen1();
        
      default:
        debugScreen1();
        break;
     }
     screenBusy = 1;
  }

 
 
}

void updateScreen()
{
  if (screenBusy)
  {
    display.display();
    screenBusy = 0;
  }
}
