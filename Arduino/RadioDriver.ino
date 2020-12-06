#include <ST7735_t3.h>
#include <st7735_t3_font_Arial.h>
#include <st7735_t3_font_ComicSansMS.h>
//#include <ST7789_t3.h>
#include <TinyGPS.h>
#include <SPI.h>

#include "devAR620x.h"
#include "BasicSerial.h"
#include "StationSelector.h"
#include "Encoder.h"
#include "Switch.h"

#define TFT_CS 10
#define TFT_DC 6 
#define TFT_RST 4
#define TFT_CS2 5
#define TFT_BKL 9

ST7735_t3 tft(TFT_CS,  TFT_DC, TFT_RST);

Serial1T radioSerial;
AR620x radio(&radioSerial);
Encoder encoder(22,23);
Switch setSwitch(21); // on encoder
Switch selectASwitch(20); // to choose selector
Switch selectBSwitch(19); // to choose selector
Switch navSwitch(18);     // to request nav details
StationSelector selector;
TinyGPS gps;
char radioBuffer[128];

void setup() {
  Serial.begin(9600);

  radioSerial.begin(9600);
  //pinMode(4,INPUT_PULLUP); // send to radio.

  Serial3.begin(9600); // GPS input

  // SPI pins
  pinMode(11,OUTPUT);
  pinMode(12,INPUT);
  pinMode(13,OUTPUT);

  // And fire up extra TFT driving pins.
  pinMode(TFT_CS2,OUTPUT);  
  pinMode(TFT_BKL,OUTPUT);
  digitalWrite(TFT_CS2,HIGH);
  digitalWrite(TFT_BKL,HIGH);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  Serial.println(tft.width());
  Serial.println(tft.height());
  
  selector.setEncoder(&encoder, &setSwitch);
  selector.setSelectionSwitches(&selectASwitch, &selectBSwitch);
  selector.setNavSwitch(&navSwitch);
  selector.init(); // now switches & screen set up.

  // DEBUG - initialise to GRL
  double grlLat = 52.0 + 10.0/60 + 45.0/3600;
  double grlLon = -(0.0 + 06.0/60 + 55.0/3600);
  selector.setPosition(grlLat, grlLon, 0);

}

void loop() {
  encoder.pulse();  
  setSwitch.pulse();
  selectASwitch.pulse();
  selectBSwitch.pulse();
  navSwitch.pulse();

  // Look for any data from radio and process it.
  size_t bytes = radioSerial.available();
  if(bytes) {
    if(bytes > sizeof(radioBuffer)){
      bytes = sizeof(radioBuffer);
    }
    bytes = radioSerial.readBytes(radioBuffer, bytes);
    radio.parseString(radioBuffer,bytes);
  }

//  if(radio.state().Changed) {
//    Serial.println(radio.state().ActiveFrequency);
//    Serial.println(radio.state().PassiveFrequency);
//  }
  // Update GPS
  // https://www.pjrc.com/teensy/td_libs_TinyGPS.html
  while (Serial3.available())  {
    int c = Serial3.read();
    if (gps.encode(c)) {
      float latitude;
      float longitude;
      unsigned long age;

      gps.f_get_position(&latitude, &longitude, &age);
      if(age != TinyGPS::GPS_INVALID_AGE && age < 10000) { // in last 10s
        selector.setPosition(latitude, longitude, age);
         // Note also!!
        // float falt = gps.f_altitude(); // +/- altitude in meters
        // float fc = gps.f_course(); // course in degrees
        // float fk = gps.f_speed_knots(); // speed in knots
        // float fmph = gps.f_speed_mph(); // speed in miles/hr
        // float fmps = gps.f_speed_mps(); // speed in m/sec
        // float fkmph = gps.f_speed_kmph(); // speed in km/hr
      }
    }
  }

  // finally allow selector to do any timing related stuff
  selector.pulse();
  
}
