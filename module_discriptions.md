# Welcome to hardware discriptions!
This is where we can list the components and assembly instructions for the circuits needed for our modules. Fully detailed schematics will be in the report.

## Gabe — Sound Machine
Components needed:
* Passive Piezo buzzer x3 (standard hobby specs no brand)
* Resistor x3 (very likely 100-200 ohms depending on desired volume and safety level) (no brand)
* Capacitor x3 -> Experement but 10 micro farads is likely good (no brand)
* Jumper wires to make connections (no brand)
Connect pin 11 to one piezo buzzer through resistor and capacitor to ground
Connect pin 5  to one piezo buzzer through resistor and capacitor to ground
Connect pin 6  to one piezo buzzer through resistor and capacitor to ground

## Roman - LCD display Logic (Tetris)
Make the following connections:
Pin 2  -> LED pin on the LCD
Pin 4  -> Rotate Peice Button
Pin 8  -> RESET pin on LCD
Pin 9  -> CD pin on LCD
Pin 10 -> CS pin on LCD
Pin 12 -> Right Button
Pin 13 -> Left Button
Pin 50 -> 
Pin 51 -> 
Pin 52 -> 

## Son and Gabe - Water Gun
Wire 2 optocouplers so they control the current flowing from REV to V- and FWD to V- on the relay, and control the optocouplers with pin 22 for the FWD optocoupler and 23 for the REV one, then wire the two putputs of the relay into the power pins actuator using a pigtail connector.

## Son - Potenteometer
Connect one side of potenteometer to ground, one to vcc, and the middle pin to analog in 0.