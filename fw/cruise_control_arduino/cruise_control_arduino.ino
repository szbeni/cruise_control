#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <STM32TimerInterrupt.h>
#include <STM32_ISR_Timer.h>
#include "ELMduino.h"
#include "throttle_pedal.h"
#include "config.h"

typedef struct {
  uint8_t mode; 
  float manualThrottle;
  float throttleInPrev;
  float throttleIn;
  float throttleOut;

  //Speed control
  float currentSpeed;
  float targetSpeed;
  float pTerm;
  float iTerm;
  float dTerm;
  float iState;
  float speedOut;
  float dT;
  float error;
  float prevError;
  
  unsigned long lastTick;
  uint8_t unit;
  
  float rpm;
  float rpmPrev;
  uint8_t brakeState;
  bool initialized = false;
  uint8_t screenMode;
  
  bool clearDTCFlag = false;
} CruiseControlStruct;

CruiseControlStruct cc;
TwoWire i2cDev(PB9, PB8);     //SDA  SCL

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &i2cDev, OLED_RESET);
ThrottlePedal pedal(DACA_ADDR, DACB_ADDR, AINA_PIN, AINB_PIN, &i2cDev);
ELM327 myELM327;
STM32Timer ITimer(TIM3);

void setup() {
  

  cruiseControlBegin();
  
  pinMode(LED_PIN, OUTPUT);


  pedal.begin();
  pedal.setCalibration(AINA_CALIB, AINB_CALIB); 
  pedal.setMinMax(AINA_MIN, AINA_MAX, AINB_MIN, AINB_MAX);
  pedal.setErrorThresholds(AINA_MAX_ERROR, AINB_MAX_ERROR, DIFFERENCE_MAX_ERROR);

  ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_MS * 1000, TimerHandler);

  Serial.begin(115200);   //USB (PA11/PA12) connected to USB

  cruiseControlSetMode(MODE_INITIALIZED);

  delay(50);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    while(1)
    {
      Serial.println(F("SSD1306 allocation failed"));
      delay(1000);
    } 
  }
  obd2Begin();
  steeringKeysBegin();
}


// Timer interrupt loop 20 Hz
static int cntr = 0;
void TimerHandler()
{
  cruiseControlLoop();  
  updateScreen();
  cntr++;
  if(cntr == 20)
  {
    cntr = 0;
    digitalToggle(LED_PIN);
  }
    
}

// Software loop
void loop() {
  obd2Loop();
  float speed = obd2GetSpeed();
  if(speed != -1)
  {
    cc.currentSpeed = speed;
  }

  float rpm = obd2GetRPM();
  if(rpm != -1)
  {
    cc.rpm = rpm;
  }

  if (cc.clearDTCFlag == true)
  {
    cc.clearDTCFlag = false;
    obd2ClearDTC();
  }

  uint8_t screen = cc.screenMode;
  if (cc.mode == MODE_LOCKED)
  {
    screen = SCREEN_MODE_LOCKED;
  }
  screenLoop(screen);
  delay(10);
}
