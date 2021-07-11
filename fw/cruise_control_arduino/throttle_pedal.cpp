#include "throttle_pedal.h"

#define CHANNEL_A 0
#define CHANNEL_B 1

ThrottlePedal::ThrottlePedal(uint8_t dacAAddr, uint8_t dacBAddr, uint8_t inputAPin, uint8_t inputBPin, TwoWire *i2cDev) {
  this->dacAAddr = dacAAddr;
  this->dacBAddr = dacBAddr;
  this->inputAPin = inputAPin;
  this->inputBPin = inputBPin;
  this->i2cDev = i2cDev;
  this->inputACalib = 1.0f;
  this->inputBCalib = 1.0f;
  this->inputAMaxError = 0.1f;
  this->inputBMaxError = 0.1f;
  this->differentialMaxError = 0.1f;
}

bool ThrottlePedal::begin() {
  analogReadResolution(12);
  pinMode(inputAPin, INPUT);
  pinMode(inputBPin, INPUT);
  return true;
}

void ThrottlePedal::setCalibration(float inputACalib, float inputBCalib)
{
  this->inputACalib = inputACalib;
  this->inputBCalib = inputBCalib;
}

void ThrottlePedal::setMinMax(float inputAMin, float inputAMax, float inputBMin, float inputBMax)
{
  this->inputAMin = inputAMin;
  this->inputAMax = inputAMax;
  this->inputBMin = inputBMin;
  this->inputBMax = inputBMax;
}

void ThrottlePedal::setErrorThresholds(float inputAMaxError, float inputBMaxError, float differentialMaxError)
{
  this->inputAMaxError = inputAMaxError;
  this->inputBMaxError = inputBMaxError;
  this->differentialMaxError = differentialMaxError;
}

void ThrottlePedal::setThrottle(float throttle) {
  if(throttle < 0.0f) {
    throttle = 0;
  }
  else if (throttle > 1.0f) {
    throttle = 1.0f;
  }
  
  float voltageA = interpolate(throttle, inputAMin, inputAMax);
  float voltageB = interpolate(throttle, inputBMin, inputBMax);
  setVoltage(CHANNEL_A, voltageA, false);
  setVoltage(CHANNEL_B, voltageB, false);
}


float ThrottlePedal::getThrottle(void) {
  float voltageA = getVoltage(CHANNEL_A);
  float voltageB = getVoltage(CHANNEL_B);
  lastVoltageA = voltageA;
  lastVoltageB = voltageB;

  float normalizedA = normalize(voltageA, inputAMin, inputAMax, inputAMaxError);  
  float normalizedB = normalize(voltageB, inputBMin, inputBMax, inputBMaxError);
  float error = normalizedA - normalizedB;

  lastThrottleA = normalizedA;
  lastThrottleB = normalizedB;

  
  //Check if reading is valid, the error between the two readings is not too much different
  if(fabs(error) > differentialMaxError)
  {
    lastThrottle = -1;
  }
  else
  {
    lastThrottle = normalizedA;
  }
  
  return lastThrottle;
}

float ThrottlePedal::normalize(float value, float minimum, float maximum, float error){
  
  if (value < minimum) 
  {
    if ((minimum - value) > error)
      return -1; 
    value = minimum;
  }
  else if (value > maximum){
    if ((value - maximum) > error)
    {
      return -2;  
    }
    value = maximum;
  }
  
  float range = maximum - minimum;
  return (value - minimum) / range;  
}

float ThrottlePedal::interpolate(float value, float minimum, float maximum)
{
  if(value < 0.0f) {
    value = 0;
  }
  else if (value > 1.0f) {
    value = 1.0f;
  }
  
  float range = (maximum - minimum);
  return minimum + range * value; 
}

float ThrottlePedal::getVoltage(uint8_t channel)
{
  uint16_t raw = 0;
  float scalar = RAW_TO_VOLTAGE_SCALAR;
  if (channel == CHANNEL_A)
  {
    raw = analogRead(inputAPin); 
    scalar *= inputACalib;
  }
  else if (channel == CHANNEL_B)
  {
    raw = analogRead(inputBPin); 
    scalar *= inputBCalib;    
  }
  return  (float)raw * scalar;
}

bool ThrottlePedal::setVoltage(uint8_t channel, float voltage, bool writeEEPROM)
{
  uint8_t addr;
  uint8_t packet[3];
  uint16_t output;

  if(channel == CHANNEL_A) 
  {
    addr = dacAAddr;
  }
  else
  {
    addr = dacBAddr; 
  }
  
  output = voltage * VOLTAGE_TO_RAW_SCALAR;
  if (output > 4095) {
    output = 4095;
  }
  
  if (writeEEPROM) {
    packet[0] = 0x60;   //Write to output and to EEPROM too
  } else {
    packet[0] = 0x40; //Write only to output
  }
  
  packet[1] = output / 16;        // Upper data bits (D11.D10.D9.D8.D7.D6.D5.D4)
  packet[2] = (output % 16) << 4; // Lower data bits (D3.D2.D1.D0.x.x.x.x)
  
  i2cDev->beginTransmission(addr);
  i2cDev->write(packet[0]);
  i2cDev->write(packet[1]);
  i2cDev->write(packet[2]);
  uint8_t retval = i2cDev->endTransmission();

  //successful
  if(retval == 0)
    return true;
    
  return false;
}
