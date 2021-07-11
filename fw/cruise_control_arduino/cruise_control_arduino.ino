#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ELMduino.h>
#include "throttle_pedal.h"


//OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


//DAC
#define DACA_ADDR 0x60
#define DACB_ADDR 0x61

//ANALOG INPUTS
#define AINA_PIN A0
#define AINA_CALIB 0.981132075f
#define AINA_MIN   0.97
#define AINA_MAX   3.74
#define AINA_MAX_ERROR 0.2

#define AINB_PIN A1
#define AINB_CALIB 0.977794891f
#define AINB_MIN   0.48
#define AINB_MAX   1.87
#define AINB_MAX_ERROR 0.2

#define DIFFERENCE_MAX_ERROR 0.4

//BUTTONS
#define BTN_NUM 2
#define BTN_UP_PIN    PB13
#define BTN_DOWN_PIN  PB12

//BRAKE PEDAL INPUT
#define BRAKE_PEDAL_PIN  PB14

//LED
#define LED_PIN PC13

#define MODE_NORMAL 0
#define MODE_MANUAL 1
#define MODE_ERROR  2
String modeName[] = {"Normal", "Manual", "Error"};

TwoWire i2cDev(PB9, PB8);     //SDA  SCL
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &i2cDev, OLED_RESET);
ThrottlePedal pedal(DACA_ADDR, DACB_ADDR, AINA_PIN, AINB_PIN, &i2cDev);
ELM327 myELM327;

void startButtons();
void buttonsLoop();

void setup() {
  pinMode(BRAKE_PEDAL_PIN, INPUT);
  pedal.begin();
  pedal.setCalibration(AINA_CALIB, AINB_CALIB); 
  pedal.setMinMax(AINA_MIN, AINA_MAX, AINB_MIN, AINB_MAX);
  pedal.setErrorThresholds(AINA_MAX_ERROR, AINB_MAX_ERROR, DIFFERENCE_MAX_ERROR);

  startButtons();
  
  Serial.begin(115200);   //USB (PA11/PA12) connected to USB
  obd2Begin();
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }    
}


void updateScreen(uint8_t mode, float voltageA, float voltageB, float inputThrottle, float outputThrottle, float manualThrottle, bool brakeState, float currentSpeed, float rpm)
{
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner

  display.print("Mode:   ");
  display.println(modeName[mode]);
  
//  display.print("Voltage A:");
//  display.print(voltageA);
//  display.print(", ");
//  display.println(pedal.lastThrottleA);
//  
//  display.print("Voltage B:");
//  display.print(voltageB);
//  display.print(", ");
//  display.println(pedal.lastThrottleB);
  display.print("In: ");
  display.print(inputThrottle);
  
  display.print("    Out:");
  display.println(outputThrottle);
  
  display.print("Man: ");
  display.print(manualThrottle);

  display.print("    Brk:");
  display.println(brakeState);

  display.print("Speed: ");
  display.println(currentSpeed);

  display.print("RPM: ");
  display.println(rpm);

  
  display.display();
}

uint8_t mode = MODE_NORMAL; 
float manualThrottle = 0.0f;

float changeThrottle(float currentValue, float change)
{
  currentValue += change;
  if (currentValue > 1.0f)
  {
    currentValue = 1.0f;
  }
  else if(currentValue < 0.0f)
  {
   currentValue = 0.0f;
  }
  return currentValue;
}


void buttonHold(uint8_t btn_num)
{
  if(btn_num==0) {
    manualThrottle = changeThrottle(manualThrottle, 0.01f);
  }
  else if(btn_num==1) {
    manualThrottle = changeThrottle(manualThrottle, -0.01f);
  }
}

void buttonShortPress(uint8_t btn_num)
{
  if(btn_num==0) {
    manualThrottle = changeThrottle(manualThrottle, 0.01f);
  }
  else if(btn_num==1) {
    manualThrottle = changeThrottle(manualThrottle, -0.01f);
  }
  
}

void buttonLongPress(uint8_t btn_num)
{
  if(btn_num==0) {
    mode = MODE_MANUAL;
  }
  else if(btn_num==1) {
    mode = MODE_NORMAL;
  }
  
}


void loop() {
  obd2Loop();
  
  float throttleIn, throttleOut, currentSpeed, rpm;
  uint8_t brakeState;
  
  currentSpeed = obd2GetSpeed();
  rpm = obd2GetRPM();
  brakeState = digitalRead(BRAKE_PEDAL_PIN);
  
  if (brakeState == HIGH)
  {
    if(mode != MODE_ERROR)
    {
      mode = MODE_NORMAL;  
    }
  }
 
  buttonsLoop();
 
  throttleIn = pedal.getThrottle();
  if(throttleIn < 0.0f)
  {
    mode = MODE_ERROR;
    throttleOut = 0;
  }
  else
  {
    //get out of error mode if pedal is detected
//    if (mode == MODE_ERROR)
//    {
//      mode = MODE_NORMAL;
//    }
  }
  
  
  if (mode == MODE_NORMAL)
  {
    throttleOut = throttleIn;
    //Hold last value
    manualThrottle = throttleIn;
    
  }
  else if (mode == MODE_MANUAL)
  {
    throttleOut = manualThrottle;
  }
  else if (mode == MODE_ERROR)
  {
    throttleOut = 0;
  }
  
  pedal.setThrottle(throttleOut);
  updateScreen(mode, pedal.lastVoltageA, pedal.lastVoltageB, throttleIn, throttleOut, manualThrottle, brakeState, currentSpeed, rpm);
  delay(20);
}
