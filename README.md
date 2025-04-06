# Atomic Clock NTP Modification
An ESP8266 Module and an Arduino sketch replace the problematic WWVB radio receiver in an Atomic Clock to synchronize the clock using NTP.

After connecting to a Wifi server, the ESP8266 then connects to an NTP server using the functions built into the ESP8266 core. It sends a digital signal to the clock controller that makes the controller think     that it's receiving the WWVB time signal from Ft. Collins CO. The ESP8266 flashes an LED on the face of the clock just because I like blinkin' lights.
<p align="center"><img src="/images/Atomic Clock 1.JPG"/>
<p align="center">'Atomix' Atomic Clock Internals</p><br>
<p align="center"><img src="/images/Atomic Clock 2.JPG"/>
<p align="center">ESP8266 and Clock Controller</p><br>
<p align="center"><img src="/images/Atomic Clock NTP Modification.png"/>
<p align="center">Connections between ESP8266 and Clock Controller</p><br>


https://github.com/user-attachments/assets/5a63e140-8412-42de-95e4-2bb3fc324b43
<p align="center">Monitoring the first 30 seconds to the time data</p><br>


https://github.com/user-attachments/assets/1b09c96e-a9e4-42d7-8834-14e9ce35e0c6
<p align="center">The Atomic Clock being updated</p><br>

