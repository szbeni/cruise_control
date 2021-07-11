#include <Wire.h>

#define RAW_TO_VOLTAGE_SCALAR  (5.0/4095.0)
#define VOLTAGE_TO_RAW_SCALAR  (4095.0/5.0)


class ThrottlePedal{
public:

  float lastVoltageA;
  float lastVoltageB;
  float lastThrottle;
  float lastThrottleA;
  float lastThrottleB;

  ThrottlePedal(uint8_t dacAAddr, uint8_t dacBAddr, uint8_t inputAPin, uint8_t inputBPin, TwoWire *i2cDev);
  bool begin();
  void setCalibration(float inputACalib, float inputBCalib);
  void setMinMax(float inputAMin, float inputAMax, float inputBMin, float inputBMax);
  void setErrorThresholds(float inputAMaxError, float inputBMaxError, float differentialMaxError);
  
  void  setThrottle(float throttle);
  float getThrottle(void);
   
private:
  TwoWire* i2cDev;
  
  uint8_t dacAAddr;
  uint8_t dacBAddr;
  uint8_t inputAPin;
  uint8_t inputBPin;

  float inputACalib;   //input A calibration
  float inputBCalib;   //input B calibration
  float inputAMin;      //input A value at zero throttle
  float inputAMax;      //input A value at full throttle
  float inputBMin;      //input B value at zero throttle
  float inputBMax;      //input B value at full throttle

  float inputAMaxError;
  float inputBMaxError;
  float differentialMaxError;
  
  float interpolate(float value, float minimum, float maximum);
  float normalize(float value, float minimum, float maximum, float error);
  
  bool setVoltage(uint8_t channel, float voltage, bool writeEEPROM = false);
  float getVoltage(uint8_t channel);
  
};
