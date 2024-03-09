# cruise_control
STM32F401 black pill based cruise control
* STM32F401CC balck pill
* 2x MCP4725 DAC converter
* LM358 opamp to scale down analog 5v to 3.3V 
* 2x push buttons
* 1x SSD1306 OLED screen
* 1x Universal Car Steering Wheel Remote Control Wireless Dvd Navigation (with analog outputs)
<img src="https://raw.githubusercontent.com/szbeni/cruise_control/main/images/buttons.jpg" width="400">
<img src="https://raw.githubusercontent.com/szbeni/cruise_control/main/images/button_receiver.png">
* 1x ELM327 Bluetooth OBD2 adapeter (Bluetooth module removed, connected directly to pic18f25k80 from blackpill)
<img src="https://raw.githubusercontent.com/szbeni/cruise_control/main/images/elm327_mini.jpg" width="200">


Only do this at your on RISK!
The throttle pedal cable needs to be cut and the module needs to be connected inline.
The brake pedal switch needs to be tapped into, to detect pressing of the brake.

The module sits inline of the throttle pedal cables. In a Peugeot Boxer 2013 a throttle pedal signal is basically 2 potentiometers (channel A and B).
In normal mode the module reads this 2 analog values and outputs them via the DAC.
In cruise control mode the module is controlling the speed by changing the throttle.
The actual speed is read from the ODB2 diagnostic bus. As soon as the user steps on the acceleration pedal or the brake pedal the unit switches back to Normal mode.

Schematic: <a href="https://raw.githubusercontent.com/szbeni/cruise_control/main/hw/cruise_control/cruise_control.pdf">cruise_control.pdf</a>

Photos of prototype:
<img src="https://raw.githubusercontent.com/szbeni/cruise_control/main/images/photo_buttons_screen.jpg" width="400">
<img src="https://raw.githubusercontent.com/szbeni/cruise_control/main/images/photo_elm327.jpg" width="400">
<img src="https://raw.githubusercontent.com/szbeni/cruise_control/main/images/photo_pedals.jpg" width="400">