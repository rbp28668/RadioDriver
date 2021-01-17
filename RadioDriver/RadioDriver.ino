// Ensure Teensy optimised version is used: https://github.com/PaulStoffregen/ST7735_t3
#include <ST7735_t3.h>
#include <st7735_t3_font_Arial.h>
#include <st7735_t3_font_ComicSansMS.h>
#include <SdFat.h>
#include <TinyGPS.h>
#include "devAR620x.h"
#include "BasicSerial.h"
#include "Encoder.h"
#include "Switch.h"
#include "RadioStation.h"
#include "Channel.h"
#include "Settings.h"
#include "StationSelector.h"
#include "CupFile.h"

// Chip select for SD card
#define SD_CS 16
// Need to slow down SPI for SD card so define SCK
#define SD_CONFIG SdSpiConfig(SD_CS, SHARED_SPI, SD_SCK_MHZ(10))

// Pins to drive TFT display
#define TFT_CS 10
#define TFT_DC 6 
#define TFT_RST 4
#define TFT_CS2 5
#define TFT_BKL 9

// Things found on SD card - bitmap
#define RADIO_STATIONS 1
#define CHANNELS 2
#define CONFIG 4

class Startup {
  public: 
  Startup();
};


Startup::Startup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  delay(1000);
}

Startup startup;

ST7735_t3 tft(TFT_CS,  TFT_DC, TFT_RST);

// Support for FAT16/FAT32 and exFAT
SdFs sd;

Channels channels;
RadioStations radioStations;

Serial1T radioSerial;
AR620x radio(&radioSerial);
Encoder encoder(23,22);
Switch setSwitch(21); // on encoder
Switch selectASwitch(20); // to choose selector
Switch selectBSwitch(19); // to choose selector
Switch navSwitch(18);     // to request nav details
TinyGPS gps;
char radioBuffer[128];
StationSelector selector(&radioStations, &channels);


static void changeSpiDevice(){
    // Turn off both devices, assume library code turns them on as needed.
  digitalWrite(TFT_CS,HIGH);
  digitalWrite(SD_CS, HIGH);
}


static bool endsWith(const char* str, const char*suffix){
  size_t len = strlen(str);
  size_t slen = strlen(suffix);

  const char* pos = str + len - slen;
  for(size_t i=0; i<slen; ++i) {
    if(pos[i] != suffix[i]){
      return false;
    }
  }
  return true;
}


static int loadStations(RadioStations& stations) {
  FsFile dir;
  FsFile file;

  int processedFile = 0;

  char name[256];
  const char* cupSuffix = ".cup";

  CupFile cup;
  
  // Open root directory
  if (dir.open("/")){
    // Open next file in root.
    // Warning, openNext starts at the current position of dir so a
    // rewind may be necessary in your application.
    while (file.openNext(&dir, O_RDONLY)) {
  
      if(!file.isHidden() && !file.isDir()){
        file.printFileSize(&Serial);
        file.printModifyDateTime(&Serial);
        file.printName(&Serial);

        file.getName(name, sizeof(name));
        if( endsWith(name, cupSuffix)) {
          cup.read(file, stations);
          processedFile |= RADIO_STATIONS;
        } 

        file.close();
      }
    }
    if (dir.getError()) {
      processedFile = false; // indeterminate state so load defaults later
    }
  }
  return processedFile;
}

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

  changeSpiDevice(); // make sure both disabled.
  
  // screen rotation & clearing clears edges properly.
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);

  // Initialize the SD and try to read files;
  int stationsRead = 0;

 
  // Read files from SD card but ignore if nav switch held down.
  if(!navSwitch.active()){
    tft.setCursor(1,65);
    tft.setFont(Arial_18);
    tft.print("Reading SD");

    changeSpiDevice();
    if (sd.begin(SD_CONFIG)) {
      stationsRead = loadStations(radioStations);
      sd.end(); // close and free memory
    } else {
      sd.initErrorPrint(&Serial);
      changeSpiDevice();
      tft.fillScreen(ST7735_BLACK);
      tft.setCursor(20,50);
      tft.setFont(Arial_18);
      tft.setTextColor(ST7735_RED);
      tft.print("NO CARD");
      tft.setCursor(35,75);
      tft.print("FOUND");
      delay(3000);
    }
    changeSpiDevice();
  }

    
  // If data not read from SD card then load up defaults.
  if( (stationsRead && RADIO_STATIONS) == 0){
    radioStations.loadDefaults();
  }
  if((stationsRead && CHANNELS) == 0){
    channels.loadDefaults();
  }
  radioStations.sortByName();

    
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