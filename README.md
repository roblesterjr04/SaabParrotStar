# SaabParrotStar
Arduino project designed to interface between Saab's onstar buttons and a parrot CK3000 evo handsfree unit.

## Operation
### Standby
|Button     |Action    |Function                 |
|:---------:|:--------:|:-----------------------:|
|*OnStar*   |Press Once|Activate Siri/Answer Call|
|--         |Hold      |Redial                   |
|Dot        |Press Once|Enter Menu               |
|--         |Hold      |Enable/Disable Aux 1     |
|Red Cross  |Press Once|--                       |
|--         |Hold      |Enable/Disable Aux 2     |
|--         |Long Hold |Reset Parrot             |

### In Call
|Button     |Action    |Function                 |
|:---------:|:--------:|:-----------------------:|
|*OnStar*   |Press Once|Call Waiting/Private Mode|
|--         |Hold      |Redial                   |
|Dot        |Press Once|Volume Up                |
|--         |Hold      |Volume Down              |
|Red Cross  |Press Once|Hang Up                  |
|--         |Hold      |Hang Up                  |

### Menu Operation
|Button     |Action    |Function                 |
|:---------:|:--------:|:-----------------------:|
|*OnStar*   |Press Once|Select                   |
|--         |Hold      |--                       |
|Dot        |Press Once|Down                     |
|--         |Hold      |--                       |
|Red Cross  |Press Once|Up                       |
|--         |Hold      |Exit                     |

## Board Version History
### Version *Prototype V1 (Main.brd)*
This board will utilize a Teensy 3.2 as the microcontroller, directly attached to the breakout board housing an AND gate for LED power, 2 relays for 12V switching, a 1K pull-down resistor for the analog button stream, and pads for the cabling to attach. The Teensy will have a USB extension running up the line to just behind the *OnStar* button set for easy programming when the unit is in the car.

### Version *Beta 1 (A328.brd)*
This board uses the ATMega328P chip and an external 16MHz crystal. Chip should be bootloaded and programmed before installed on the board. The traces for the 12V switching relays are rated for 1.42A, though the G5V1 relays are only rated at 1A. All traces can handle up to 440mA. The lettering on the connectors are wire color for the 4 pin cable, and by purpose on the 5 pin. The pinout for the 5 pin is so: *P = Power/5v, E = Earth/Gnd, A = Analog signal, G = Green Led, R = Red Led*. The ATMega328P has only one serial port, so the code must be modified to use Serial0, not Serial1 as it is now for development/prototype. Additional programming can be done by using a USB to serial breakout board from SparkFun and connecting to the RX/TX lines in the 4 pin molex (Red/TX and blue/RX).

## Future Ideas...
Will like to build CK3100 compatability to allow for text display to be piped in to CAN bus and display on Saab's SID display. Would also like to conceive a method to integrate steering wheel controls. Should not be too challenging since they are also an analog circuit.
