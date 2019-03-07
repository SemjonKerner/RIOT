# Example: Ultrasonic Sensor <-> Audio Buzzer
## Brief Description
In this simple example an ultrsaound distance sensor and an audio buzzer are
arranged in order to playfully present wireless communication in RIOT.
This example was initially created for the lange Nacht der Wissenschaften 2018
at FU Berlin. It uses a gnrc server waiting for distance values and a
corresponding gnrc client reading distance values from an ultrasonic module.
The server will then convert the distance values into notes and play them on
a buzzer via pwm.
### Pictures
![wat][calliope]
![wat][nrf52dk]
![wat][paddle]
### Usage
You can control the distance to the ultrasonic sensor with Hands or preferably
with a tabletennis paddle. The microcontrollers will then exchange these
distance values. The server then calculates a note from that distance and the
pwm signal to play this via the buzzer.
## Partlist
| # | HW | Name  | Description | Firmware |
|---|---|---|---|---|
| 1 | BOARD  | calliope-mini  | Calliope educational board | [buzzer directory][2]|
| 1 | BOARD  | nrf52dk  | Nordic nrf52 developement kit | [sensor directory][3] |
| 1 | MODULE | HC-SR04  | Ultrasonic distance sensor | N/A |
| 1 | MODULE | YL-44    | simple buzzer, controlled with PWM | N/A |
| 7 | WIRE   | jumperwire | female to female | N/A |
## Setup
### Installation
The code resides in folders [/buzzer][2] and [/sensor][3].  
The buzzer is build and flashed on calliope-mini:  
`PORT=/dev/ttyACM0 BOARD=calliope-mini make flash term`

The Sensor is build and flashed on nrf52dk:  
`PORT=/dev/ttyACM1 BOARD=nrf52dk make flash term`

---

**NOTE**

The nrf52dk has to be reset after poweron to work properly with the srf04
driver. See [nrf52_resetpin_cfg][7] if the reset button does not work out of
the box.  
*Apparently this problem is created by a capacitive load on the GPIO port of the
echo channel. This issue could be reproduced and also fixed with longer
jumperwires. However a reset is recommended in such a case.*

---
### Pinout
|Board|Pin|Module|Pin|
|---|---|---|---|
| [Calliope][1] | 3V3 | Buzzer YL-44 | Vcc |
|          | 0   | | I/0 |
|          | GND | | GND |
| nrf52dk  | 5V  | Sensor HC-SR04 | Vcc |
|          | 13  | | Trigger |
|          | 14  | | Echo |
|          | GND | | GND |

The pins for PWM on calliope are set to default in the [boards config][5].  
The pins for the srf04 driver are set to default in the [srf04 driver headers][6].
## Reference
[1]: https://meingottundmeinewelt.de/2017/05/22/calliope-mini-go/
[2]: https://github.com/SemjonKerner/RIOT/tree/ln18/examples/ln18/buzzer
[3]: https://github.com/SemjonKerner/RIOT/tree/ln18/examples/ln18/sensor
[4]: https://github.com/RIOT-OS/RIOT/tree/master/drivers/srf04
[5]: https://github.com/RIOT-OS/RIOT/blob/master/boards/calliope-mini/include/periph_conf.h
[6]: https://github.com/SemjonKerner/RIOT/blob/ln18/drivers/srf04/include/srf04_params.h
[7]: https://github.com/RIOT-OS/RIOT/tree/master/dist/tools/nrf52_resetpin_cfg
[calliope]: pic/calliope.jpg "calliope-mini with buzzer"
[nrf52dk]: pic/nrf52dk.jpg "nrf52dk with sensor"
[paddle]: pic/paddle.jpg "paddle"

\[1] https://meingottundmeinewelt.de/2017/05/22/calliope-mini-go/  
\[2] https://github.com/SemjonKerner/RIOT/tree/ln18/examples/ln18/buzzer  
\[3] https://github.com/SemjonKerner/RIOT/tree/ln18/examples/ln18/sensor  
\[4] https://github.com/SemjonKerner/RIOT/tree/ln18/drivers/srf04  
\[5] https://github.com/RIOT-OS/RIOT/blob/master/boards/calliope-mini/include/periph_conf.h  
\[6] https://github.com/SemjonKerner/RIOT/blob/ln18/drivers/srf04/include/srf04_params.h  
\[7] https://github.com/RIOT-OS/RIOT/tree/master/dist/tools/nrf52_resetpin_cfg
