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
|Dot        |Press Once|Volume Down              |
|--         |Hold      |Enter Menu               |
|Red Cross  |Press Once|Volume Up                |
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
### Version *Prototype V1*
This board will utilize a Teensy 3.2 as the microcontroller, directly attached to the breakout board housing an AND gate for LED power, 2 relays for 12V switching, a pull-down resistor for the analog button stream, and pads for the cabling to attach. The Teensy will have a USB extension running up the line to just behind the *OnStar* button set for easy programming when the unit is in the car.

### Version *Beta 1*
Not designed yet. Will not have USB breakout. Will likely use Atmega328 chip as microcontroller. Chip will be bootloaded and programmed on an Arduino board, then installed. Additional USB programming will be done through the molex connector using a USB breakout board obtained from SparkFun.

## Future Ideas...
Will like to build CK3100 compatability to allow for text display to be piped in to CAN bus and display on Saab's SID display. Would also like to conceive a method to integrate steering wheel controls. Should not be too challenging since they are also an analog circuit.
