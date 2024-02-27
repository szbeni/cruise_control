#pragma once

#define VERSION 1.4

//LIMITS
#define THROTTLE_MIN 0.0f
#define THROTTLE_MAX 1.0f

#define SPEED_MIN 5.0f
#define SPEED_MAX 120.0f


//MODES

#define CC_DEBUG_ENABLE 0

#define MODE_NOT_INITIALIZED 0
#define MODE_INITIALIZED 1
#define MODE_LOCKED 2
#define MODE_NORMAL 3
#define MODE_THROTTLE 4
#define MODE_RPM 5
#define MODE_SPEED 6
#define MODE_ERROR  7

const String modeName[] = {"Not initialized", "Initialized", "Lock", "Normal", "Throttle", "RPM", "Speed", "Error"};

//Screen modes
#define SCREEN_MODE_DEBUG1   0
#define SCREEN_MODE_DEBUG2   1
#define SCREEN_MODE_SPEED    2
#define SCREEN_MODE_SETTINGS 3
#define SCREEN_LOOP_NUM 4

#define SCREEN_MODE_LOCKED   5


//Unit modes
#define UNIT_MPH  0
#define UNIT_KMPH 1

//Keys
#define SHORT_PRESS       0
#define LONG_PRESS        1
#define HOLD_START        2
#define HOLD_STOP         3




#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_MODE 2
#define KEY_DISPLAY 3
#define KEY_UNIT 4

#define KEY_SPARE1 5
#define KEY_SPARE2 6
#define KEY_SPARE3 7
#define KEY_SPARE4 8
#define KEY_SPARE5 9




//Hardware timer
#define HW_TIMER_INTERVAL_MS 50


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

#define KEY0_PIN A4
#define KEY1_PIN A5


//BUTTONS
#define BTN_NUM 2
#define BTN_UP_PIN    PB13
#define BTN_DOWN_PIN  PB12

//BRAKE PEDAL INPUT
#define BRAKE_PEDAL_PIN  PB14
#define ANALOG_MUX_PIN PB15

//LED
#define LED_PIN PC13
