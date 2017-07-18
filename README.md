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
|Red Cross  |Press Once|Hang Up                  |
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
### Version *1.0 (A328.brd)*
This board uses the ATMega328P chip and an external 16MHz crystal. Chip should be bootloaded and programmed before installed on the board. The traces for the 12V switching relays are rated for 1.42A, though the G5V1 relays are only rated at 1A. All traces can handle up to 440mA. The lettering on the connectors are wire color for the 4 pin cable, and by purpose on the 5 pin. The pinout for the 5 pin is so: *P = Power/5v (out), E = Earth/Gnd, A = Analog signal, G = Green Led, R = Red Led*. The pinout for the 4 pin molex is so: *Y = Power/12v (in), B = Earth/Gnd, R = TX, Bl = RX*. The ATMega328P has only one serial port, so the code must be modified to use Serial0, not Serial1 as it is now for development/prototype. The chip needs to be mounted in a socket, as 2 of the resistors are placed under the ship in the recesses of the socket.

<a href="https://oshpark.com/shared_projects/tsGGRMay"><img src="https://oshpark.com/assets/badge-5b7ec47045b78aef6eb9d83b3bac6b1920de805e9a0c227658eac6e19a045b9c.png" alt="Order from OSH Park"></img></a>

## Future Ideas...
Will like to build CK3100 compatability to allow for text display to be piped in to CAN bus and display on Saab's SID display. Would also like to conceive a method to integrate steering wheel controls. Should not be too challenging since they are also an analog circuit.
