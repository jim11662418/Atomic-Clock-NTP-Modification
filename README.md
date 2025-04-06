# Atomic Clock NTP Modification
An ESP8266 Module and an Arduino sketch replace the problematic WWVB radio receiver in an Atomic Clock to synchronize the clock using NTP.

After connecting to a Wifi server, the ESP8266 then connects to an NTP server using the functions built into the ESP8266 core. It sends a digital signal to the clock controller that makes the controller think     that it's receiving the WWVB time signal from Ft. Collins CO. The ESP8266 flashes an LED on the face of the clock just because I like blinkin' lights.
<p align="center"><img src="/images/Atomic Clock 1.JPG"/>
<p align="center">'Atomix' Atomic Clock Internals</p><br>
<p align="center"><img src="/images/Atomic Clock 2.JPG"/>
<p align="center">ESP8266 and Clock Controller</p><br>
<p align="center"><img src="/images/Atomic Clock NTP Modification.png"/>
<p align="center">Connections between ESP8266 and Clock Controller</p><br>
