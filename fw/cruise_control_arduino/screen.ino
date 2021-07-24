
void debugScreen()
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
  display.setTextSize(4);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);  

  char buf[16];
  sprintf(buf, " %03d", (int)cc.targetSpeed);
  display.print(buf);
  display.setCursor(95,22);
  display.setTextSize(1);
  display.print("km/h");


   display.setCursor(0,34);
  display.print("Actual: ");
  display.print(cc.currentSpeed);

  display.setCursor(0,56);
  display.print("Mode: ");
  display.print(modeName[cc.mode]);
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
      case SCREEN_MODE_DEBUG:
        debugScreen();
        break;
      default:
        debugScreen();
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
