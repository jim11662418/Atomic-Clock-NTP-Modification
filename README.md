# Atomic Clock NTP Modification
An ESP8266 Module and an Arduino sketch replace the problematic WWVB radio receiver in an Atomic Clock to synchronize the clock using the Network Time Protocol.

After connecting to WiFi, the ESP8266 then connects to an NTP server using the functions built into the ESP8266 core. Once it's synced with the NTP server, the ESP8266 sends a digital signal to the clock controller that makes the controller think that it's receiving the WWVB time signal from Ft. Collins CO. The ESP8266 flashes an LED on the face of the clock just because I like blinkin' lights.
<p align="center"><img src="/images/Atomic Clock 1.JPG"/>
<p align="center">'Atomix' Atomic Clock Internals</p><br>
<p align="center"><img src="/images/Atomic Clock 2.JPG"/>
<p align="center">ESP8266 and Clock Controller</p><br>
The ESP8266 has three connections to the clock controller PCB (see below). One output from the ESP8266 (the sketch uses D1) connects to the 'TCO' input of the clock controller. This ESP8266 output provides the signal that would normally come from the radio reciever to indicate the WWVB signal is being received. The second connection comes from 'PON' on the controller to an input on the ESP8266. The clock controller pulls this line low to turn on the radio receiver. This input tells the ESP8266 that the clock controller is actively seeking the WWVB signal. Most clocks turn on their radios and look for a signal only once every 24 hours, usually at night. The sketch only uses this input to suppress the flashing LED. The third connection between the two boards is ground.<br>

<br><p align="center"><img src="/images/Atomic Clock NTP Modification.png"/>
<p align="center">Connections between ESP8266 and Clock Controller</p><br>


https://github.com/user-attachments/assets/5a63e140-8412-42de-95e4-2bb3fc324b43
<p align="center">Monitoring the first 30 seconds of the time signal output from the ESP8266 to the clock controller</p><br>


https://github.com/user-attachments/assets/1b09c96e-a9e4-42d7-8834-14e9ce35e0c6
<p align="center">The 'WAVE' icon on the clock face stops flashing when the clock is synced</p><br>

