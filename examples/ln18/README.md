examples/ln18
================
This is a simple example for lange Nacht der Wissenschaften 2018 in Berlin.
It uses a gnrc server playing noise over nrf51 pwm driver and a client
recieving distance values from an srf04 ultrasonic modul.
The Server runs on the calliope-mini [1] board, or another with nrf51 CPU.
It uses 3V3, GND and Pin 0 for the PWM out.
The Client runs on nrf52dk, because it needs 5V for the distance sensor.
It uses 5V, GND, Pin 13 for the Trigger and Pin 14 for Echo.

[1] https://meingottundmeinewelt.de/2017/05/22/calliope-mini-go/

Usage
=====

Build, flash and start the applications in buzzer and sensor:

    PORT=/dev/ttyACM0 [ttyACM1] make [-j 4] flash term
