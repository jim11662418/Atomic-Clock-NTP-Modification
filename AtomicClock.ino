//--------------------------------------------------------------------------
// This sketch uses an ESP8266 module to replace the radio reciever in an 'Atomic" clock.
// The sketch:
//   1. connects to WiFi
//   2. connects to an NTP server using the functions built into the ESP8266 core
//   3. sends a digital signal to the clock controller that makes the controller think
//      that it's receiving the WWVB time signal from Ft. Collins CO.
//   4. flashes an LED on the face of the clock just because.
//--------------------------------------------------------------------------

#include <ESP8266WiFi.h>   
#include <Ticker.h>    

#define WIFISSID "********"
#define PASSWORD "********"

//#define NTPSERVERNAME "time.aws.com"
//#define NTPSERVERNAME "time.cloudflare.com"
//#define NTPSERVERNAME "time.apple.com"
//#define NTPSERVERNAME "north-america.pool.ntp.org"
//#define NTPSERVERNAME "time.nist.gov"
  #define NTPSERVERNAME "time.windows.com"
//#define NTPSERVERNAME "time.google.com"
//#define NTPSERVERNAME "time-a-g.nist.gov"

#define MYTZ "EST5EDT,M3.2.0,M11.1.0"                    // Eastern Standard Time/Eastern Daylight Saving Time

// NodeMCU pins
#define D0 16                                            // LED_BUILTIN on the left side of NodeMCU
#define D1  5                                            // I2C Bus SCL (clock)
#define D2  4                                            // I2C Bus SDA (data)
#define D3  0
#define D4  2                                            // blue LED on the right side of NodeMCU
#define D5 14                                            // SPI Bus SCK
#define D6 12                                            // SPI Bus MISO 
#define D7 13                                            // SPI Bus MOSI RX1
#define D8 15                                            // SPI Bus CS   TX1
#define D9  3                                            // RX0 (Serial console)
#define D10 1                                            // TX0 (Serial console) 

#define ledPin      D0                                   // output to control the built-in blue LED and the green LED on the front of the clock
#define radioPin    D1                                   // simulates the output from WWVB radio receiver - goes high to indicate modulated carrier being recieved 
#define powerPin    D2                                   // input monitors the signal from the clock controller which powers up the radio receiver
#define triggerPin  D3                                   // output goes low to trigger the logic analyzer

// Global time variables
time_t NOW;
int gmtSecond;
int gmtMinute;
int gmtHour;
int gmtDay;
int gmtMonth;
int gmtYear;

Ticker ledTimer;                                         // turns the LED off after 100 ms

void setup() {

  pinMode(powerPin,INPUT);
  pinMode(ledPin,OUTPUT); 
  digitalWrite(ledPin,HIGH);
  pinMode(radioPin,OUTPUT);
  digitalWrite(radioPin,LOW);
  pinMode(triggerPin,OUTPUT);
  digitalWrite(triggerPin,HIGH);
  
  Serial.begin(115200);  
  unsigned long waitTime = millis()+500;
  while(millis() < waitTime)yield();                     // wait one half second for the serial port
  Serial.println("\n\nNTP Atomic Clock Controller");
  Serial.printf("Sketch size: %u\n",ESP.getSketchSize());
  Serial.printf("Free size: %u\n",ESP.getFreeSketchSpace());
  Serial.print(ESP.getResetReason());
  Serial.println(" Reset");

  ConnectWiFi();
  initNTP();
  Serial.println("Local time now is "+dateTimeStr(NOW));
  Serial.println("setup() done\n");
}

void loop() {
  static unsigned long lastTime = 0; 
  int dofy,yr;

  NOW = time(nullptr);                                
  if (NOW > lastTime) {                                  // if one second has elapsed...                   
    lastTime=NOW;

    getGMTtime();                                        // update time vars needed below
    yr = gmtYear-2000;                                   // 2 digit year
    dofy = getDayofYear(gmtDay,gmtMonth,gmtYear);        // day of the year

// Each minute, WWVB broadcasts the current time in a binary-coded decimal format based on the IRIG timecode.
// Markers are sent during seconds 0, 9, 19, 29, 39, 49 and 59 of each minute. Thus, the start of the second of 
// two consecutive markers indicates the top of the minute, and serves as the on-time marker for the next frame of
// time code. Markers are important to allow receivers to properly frame the time code. There are 11 unused bits, 
// transmitted as binary 0. The remaining 42 bits, zeros and ones, carry the binary time code and other information.
// see https://www.nist.gov/pml/time-and-frequency-division/time-distribution/radio-station-wwvb/wwvb-time-code-format

    switch (gmtSecond) {
       case 0:
          digitalWrite(triggerPin,LOW);                  // trigger the logic analyzer
          marker();                                      // send 'marker'
          break;
       case 1:
          (((gmtMinute/10)>>2) & 1) ? one() : zero();    // minute 40
          break;
       case 2:
          (((gmtMinute/10)>>1) & 1) ? one() : zero();    // minute 20
          break;
       case 3:
          (((gmtMinute/10)>>0) & 1) ? one() : zero();    // minute 10
          break;
       case 4:
          zero();                                        // always zero
          break;
       case 5:
          (((gmtMinute%10)>>3) & 1) ? one() : zero();    // minute 8
          break;
       case 6:
          (((gmtMinute%10)>>2) & 1) ? one() : zero();    // minute 4
          break;
       case 7:
          (((gmtMinute%10)>>1) & 1) ? one() : zero();    // minute 2
          break;
       case 8:
          (((gmtMinute%10)>>0) & 1) ? one() : zero();    // minute 1
          break;
       case 9:
          marker();                                      // send 'marker'
          break;
       case 10:
          zero();                                        // always zero
          break;
       case 11:
          zero();                                        // always zero
          break;
       case 12:
          (((gmtHour/10)>>1) & 1) ? one() : zero();      // hour 20
          break;
       case 13:
          (((gmtHour/10)>>0) & 1) ? one() : zero();      // hour 10
          break;
       case 14:
          zero();                                        // always zero
          break;
       case 15:
          (((gmtHour%10)>>3) & 1) ? one() : zero();      // hour 8
          break;
       case 16:
          (((gmtHour%10)>>2) & 1) ? one() : zero();      // hour 4
          break;
       case 17:
          (((gmtHour%10)>>1) & 1) ? one() : zero();      // hour 2
          break;
       case 18:
          (((gmtHour%10)>>0) & 1) ? one() : zero();      // hour 1
          break;
       case 19:
          marker();                                      // send 'marker'
          break;
       case 20:
          zero();                                        // always zero
          break;
       case 21:
          zero();                                        // always zero
          break;
       case 22:
          (((dofy/100)>>1) & 1) ? one() : zero();        // day 200
          break;
       case 23:
          (((dofy/100)>>0) & 1) ? one() : zero();        // day 100
          break;
       case 24:
          zero();                                        // always zero
          break;
       case 25:
          ((((dofy/10)%10)>>3) & 1) ? one() : zero();    // day 80
          break;
       case 26:
          ((((dofy/10)%10)>>2) & 1) ? one() : zero();    // day 40
          break;
       case 27:
          ((((dofy/10)%10)>>1) & 1) ? one() : zero();    // day 20
          break;
       case 28:
          ((((dofy/10)%10)>>0) & 1) ? one() : zero();    // day 10
          break;
       case 29:
          marker();                                      // send 'marker'
          break;
       case 30:
          (((dofy%10)>>3) & 1) ? one() : zero();         // day 8
          break;
       case 31:
          (((dofy%10)>>2) & 1) ? one() : zero();         // day 4
          break;
       case 32:
          (((dofy%10)>>1) & 1) ? one() : zero();         // day 2
          break;
       case 33:
          (((dofy%10)>>0) & 1) ? one() : zero();         // day 1
          break;
       case 34:
          zero();                                        // always zero
          break;
       case 35:
          zero();                                        // always zero
          break;
       case 36:
          zero();                                        // DUT1 sign
          break;
       case 37:
          one();                                         // DUT1 sign
          break;
       case 38:
          zero();                                        // DUT1 sign
          break;
       case 39:                  
          marker();                                      // send 'marker'
          break;
       case 40:
          zero();                                        // DUT1 0.8
          break;
       case 41:
          zero();                                        // DUT1 0.4
          break;
       case 42:
          zero();                                        // DUT1 0.2
          break;
       case 43:
          zero();                                        // DUT1 0.1
          break;
       case 44:
          zero();                                        // always zero
          break;
       case 45:
          (((yr/10)>>3) & 1) ? one() : zero();           // year 80
          break;
       case 46:
          (((yr/10)>>2) & 1) ? one() : zero();           // year 40
          break;
       case 47:
          (((yr/10)>>1) & 1) ? one() : zero();           // year 20
          break;
       case 48:
          (((yr/10)>>0) & 1) ? one() : zero();           // year 10
          break;
       case 49:
          marker();                                      // send 'marker'
          break;
       case 50:          
          (((yr%10)>>3) & 1) ? one() : zero();           // year 8
          break;
       case 51:
          (((yr%10)>>2) & 1) ? one() : zero();           // year 4
          break;
       case 52:
          (((yr%10)>>1) & 1) ? one() : zero();           // year 2
          break;
       case 53:
          (((yr%10)>>0) & 1) ? one() : zero();           // year 1
          break;
       case 54:
          zero();                                        // always zero
          break;
       case 55:                                        
          (is_leap_year()) ? one() : zero();             // leap year
          break;
       case 56:
          zero();                                        // leap second
          break;
       case 57:     
          (DST_tomorrow()) ? one() : zero();             // DST status
          break;
       case 58:
          (DST_today()) ? one() : zero();                // DST status
          break;
       case 59:
          marker();                                      // send 'marker'
          break;
      }            
  } 
} // end of loop()

//------------------------------------------------------------------------
// connect to WiFi server
// the green LED flashes once every 1/4 second while attempting to connect.
// if unable to connect after 60 seconds, restart the ESP8266.
//-------------------------------------------------------------------------
void ConnectWiFi() {
  Serial.printf("\nConnecting to %s",WIFISSID);
  unsigned int waitTime=0;  
  unsigned long previousMillis=0;
  unsigned long starttime=millis();
  WiFi.begin(WIFISSID,PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {                // while waiting to connect to WiFi...
    yield();    
    unsigned long currentMillis=millis();
    if (currentMillis>previousMillis) {                  // if 1 millisecond has elapsed...
      previousMillis=currentMillis; 
      if (++waitTime>60000) ESP.restart();               // if WiFi not connected after 60 seconds, restart the ESP8266      
      if (waitTime%500==0) Serial.print(".");            // every 500 milliseconds print '.'
      if (waitTime%250==0) {
        digitalWrite(ledPin,LOW);                        // flash the green LED every 250mS
        ledTimer.once_ms(20,outputsOFF);                 // turn off the LED after 20mS
      }
    }
  }  
  Serial.printf("\nConnection to %s took %s ms\n",WIFISSID,String(millis()-starttime));
}

//------------------------------------------------------------------------
// connect to the NTP server
// the green LED flashes once every second while attempting to connect.
// if unable to connect after 60 seconds, restart the ESP8266.
//-------------------------------------------------------------------------
void initNTP() {
  Serial.printf("\nConnecting to %s",NTPSERVERNAME);
  unsigned int waitTime=0;  
  unsigned long previousMillis=0;
  unsigned long starttime=millis();  
  configTime(MYTZ,NTPSERVERNAME);
  while((NOW=time(nullptr)) < 1702684800) {              // while waiting for time to be set to later than 00:00:00 12/16/2023 GMT...
    yield();
    unsigned long currentMillis=millis();
    if (currentMillis>previousMillis) {                  // if 1 millisecond has elapsed...
      previousMillis=currentMillis;   
      if (++waitTime>60000) ESP.restart();               // if not connected to NTP server after 60 seconds, restart the ESP8266      
      if (waitTime%500==0) Serial.print(".");            // every 500 milliseconds print '.'
      if (waitTime%1000==0) {
        digitalWrite(ledPin,LOW);                        // flash the green LED once every second
        ledTimer.once_ms(20,outputsOFF);                 // turn off the LED after 20mS
      }
   }
  }
  Serial.printf("\nConnection to %s took %s ms\n",NTPSERVERNAME,String(millis()-starttime));  
}

//------------------------------------------------------------------------
// The standard value for the NTP polling interval is one hour. This value 
// can be changed by adding the following function definition to the sketch. 
// It suffices that the definition is available in the sketch, the library 
// will call the function automatically.
//------------------------------------------------------------------------
uint32_t sntp_update_delay_MS_rfc_not_less_than_15000() {
  return 10 * 60 * 1000UL;                               // 10 minutes
}

//------------------------------------------------------------------------
// update global GMT vars
//-------------------------------------------------------------------------
void getGMTtime() {
  tm *gmtNOW; 
  gmtNOW     = gmtime(&NOW); 
  gmtSecond  = gmtNOW->tm_sec;
  gmtMinute  = gmtNOW->tm_min;
  gmtHour    = gmtNOW->tm_hour;
  gmtDay     = gmtNOW->tm_mday;
  gmtMonth   = gmtNOW->tm_mon+1;
  gmtYear    = gmtNOW->tm_year+1900;
}

//------------------------------------------------------------------------
// return local date and time with format hh:mm:ss mm/dd/yyyy
//-------------------------------------------------------------------------
String dateTimeStr(time_t epochtime) {
    tm *lt;
    lt = localtime(&epochtime);
    char buff[30];
    strftime(buff,30,"%H:%M:%S %m/%d/%Y",lt);
    return buff;
}

//------------------------------------------------------------------------
// return true if DST is in effect today
//-------------------------------------------------------------------------
boolean DST_today() {
    tm *lt;
    time_t current;
    current = time(NULL);
    lt = localtime(&current);
    return lt->tm_isdst;
}

//------------------------------------------------------------------------
// return true if DST will be in effect tomorrow
//-------------------------------------------------------------------------
boolean DST_tomorrow() {
    tm *lt;
    time_t current,tomorrow;
    current = time(NULL);
    tomorrow = current+86400;
    lt = localtime(&tomorrow);
    return lt->tm_isdst;
}

//------------------------------------------------------------------------
// return true during leap year
//-------------------------------------------------------------------------
boolean is_leap_year() {
   return gmtYear%4==0 && (gmtYear%100 != 0 || gmtYear%400==0);
}

//------------------------------------------------------------------------
// return the day of the year
//-------------------------------------------------------------------------
int getDayofYear(int day,int month, int year){
   int mo[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
   int i;
   int dofy=0;
   for (i=0; i<(month-1); i++) {
      dofy += mo[i];
   }
   if (is_leap_year()) dofy++;                           // add one day for Feb 29th
   dofy += day;
   return dofy;
}   

// WWVB transmits a digital serial time code at the data rate of 1 Hz. The 60 kHz carrier power is reduced
// by 10 dB for a fraction of a second at the beginning of each UTC second. The duration of the reduced power
// determines the datum: a "0" bit is transmitted by reducing power for 200 ms and a "1" bit by reducing power
// for 500 ms. To aid in frame synchronization a marker bit is transmitted by reducing power for 800 ms. 
// There is one marker bit every 10 seconds and two back-to-back markers at the end of each minute.
//
// So a snapshot of the WWVB time code signal under perfect reception looks like a series of 200, 500 and 800
// millisecond pulses at a 1 Hz rate. Most WWVB receiver modules output logic high when carrier power is 
// reduced and logic low when the WWVB signal is at its normal full power.
// see https://en.wikipedia.org/wiki/WWVB

//------------------------------------------------------------------------
// generate 800ms 'marker' bit
//-------------------------------------------------------------------------
void marker() {
   char buff[2];
   digitalWrite(radioPin,HIGH);   
   if (!digitalRead(powerPin)) digitalWrite(ledPin,LOW); // flash the green LED only if the radio receiver is powered up (powerPin input is LOW)
   ledTimer.once_ms(800,outputsOFF);                     // turn the LED, radio and trigger outputs off after 800mS   
   sprintf(buff,"%2d",gmtSecond);   
   Serial.print(buff);   
   Serial.println(" M");
}

//------------------------------------------------------------------------
// generate 500ms 'one' bit
//-------------------------------------------------------------------------
void one() {
   char buff[2];
   digitalWrite(radioPin,HIGH);
   if (!digitalRead(powerPin)) digitalWrite(ledPin,LOW); // flash the green LED only if the radio receiver is powered up (powerPin input is LOW)
   ledTimer.once_ms(500,outputsOFF);                     // turn the LED, radio and trigger outputs off after 500mS    
   sprintf(buff,"%2d",gmtSecond);
   Serial.print(buff);   
   Serial.println(" 1");   
}

//------------------------------------------------------------------------
// generate 200ms 'zero' bit
//-------------------------------------------------------------------------
void zero() {
   char buff[2];
   digitalWrite(radioPin,HIGH); 
   if (!digitalRead(powerPin)) digitalWrite(ledPin,LOW); // flash the green LED only if the radio receiver is powered up (powerPin input is LOW)
   ledTimer.once_ms(200,outputsOFF);                     // turn the LED, radio and trigger outputs off after 200mS     
   sprintf(buff,"%2d",gmtSecond);
   Serial.print(buff);   
   Serial.println(" 0");   
}

//------------------------------------------------------------------------
// Ticker callback that turns off the LED, radio and trigger outputs
//-------------------------------------------------------------------------
void outputsOFF(){
   digitalWrite(radioPin,LOW);
   digitalWrite(ledPin,HIGH);                          
   digitalWrite(triggerPin,HIGH);
}