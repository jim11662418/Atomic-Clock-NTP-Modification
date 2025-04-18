# Atomic Clock NTP Modification
The problematic WWVB radio receiver inside an atomic clock is removed and replaced with an ESP8266 Module and an Arduino sketch which synchronize the clock using the Network Time Protocol. Although the details show here are for an 'Atomix' brand atomic clock, this hack should be applicable to many clocks that use the older amplitude modulated format.

After connecting to WiFi, the ESP8266 then connects to an NTP server. Once it's synced with the NTP server, the ESP8266 sends a digital time signal to the clock controller that mimics the signal the controller would recieve from its radio. Thus the clock controller thinks that it's receiving the WWVB time signal from Fort Collins, Colorado. The ESP8266 flashes an LED on the face of the clock as a visual indication that it's updating the clock.


https://github.com/user-attachments/assets/3439c81a-85bc-4c9b-a2b1-a6189ed5eb70
<p align="center">The 'WAVE' icon on the clock face stops flashing when the clock is synced</p><br>

<p align="center"><img src="/images/Atomic Clock 1.JPG"/>
<p align="center">'Atomix' Atomic Clock Internals</p><br>

<p align="center"><img src="/images/Atomic Clock 5.JPEG"/>
<p align="center">WWVB Receiver and Antenna Removed from Clock</p><br>

<p align="center"><img src="/images/Atomic Clock 2.JPG"/>
<p align="center">RobotDyn WiFi NodeM ESP8266 module and 'Atomix' Clock Controller</p><br>

There are three connections between the ESP8266 and the clock controller PCB (see below). One connection is an digital output from the ESP8266 to the 'TCO' input of the clock controller. This ESP8266 output provides the signal that would normally come from the radio reciever to indicate the WWVB signal is being received. The second connection comes from the 'PON' output on the controller to an digital input on the ESP8266. The clock controller pulls this line low to turn on its radio receiver. This input signals to the ESP8266 that the clock controller is actively seeking the WWVB signal. Most clocks turn on their radios and look for a signal only once every 24 hours, usually at night when the reception is better. The third connection is ground.<br>

<br><p align="center"><img src="/images/Atomic Clock NTP Modification.png"/>
<p align="center">Connections between RobotDyn WiFi NodeM ESP8266 module and the 'Atomix' Clock Controller</p><br>

# Version 2

In version 2 of the modification the project's hardware was completely revamped. The goal was to make the ESP8266 run from the clock's 2 AA batteries instead of requiring an external connection for power. To that end the power hungry RobotDyn WiFi NodeM ESP8266 module was replaced with a more frugal ESP-01 module. The red LED on the ESP-01 module that lights up to indicate power is applied was removed to reduce current consumption.

The revised sketch (AtomicClock2.ino) puts the the ESP8266 module into 'Deep Sleep' mode when the 'PON' input from the clock controller is high. The ESP8266 wakes from deep sleep when the clock controller pulls the 'PON' line low looking for a radio signal from WWVB. The ESP8266 wakes up, connects to WiFi, interrogates the NTP server, sends the time data to the clock controller and then, when the clock controller signals that it's been updated by setting the 'PON' input high, returns to deep sleep. The whole process happens at midnight and takes but a few minutes. The ESP-01 and the clock controller together consume about 75 mA when the ESP-01 is awake. The current drops to about 60 µA when the ESP-01 goes into deep sleep.

<p align="center"><img src="/images/Atomic Clock 6.JPG"/>
<p align="center">'Atomix' Atomic Clock Internals Version 2</p><br>

<p align="center"><img src="/images/Atomic Clock 7.JPG"/>
<p align="center">ESP-01 and 'Atomix' clock controller</p><br>

<p align="center"><img src="/images/Atomic Clock 8.JPG"/>
<p align="center">ESP-01 Mounted on a Piece of Perfboard</p><br>

<p align="center"><img src="/images/Atomic Clock NTP Modification V2.0.png"/>
<p style="text-align:justify;">Connections between ESP-01 and the 'Atomix' clock controller. Note the use of a resistor, capacitor and CD4049 CMOS Hex Inverter to generate a 'RST' signal to wake the ESP-01 from Deep Sleep mode.</p><br>

https://github.com/user-attachments/assets/5a63e140-8412-42de-95e4-2bb3fc324b43
<p align="center">Monitoring the first 30 seconds of the time signal output from the ESP8266 to the clock controller</p><br>



