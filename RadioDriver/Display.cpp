#include <string.h>
#include "Display.h"

#define ST7735 1

#ifdef ST7735

#include <ST7735_t3.h>  // See https://github.com/PaulStoffregen/ST7735_t3
#include <st7735_t3_font_Arial.h>
#include <st7735_t3_font_ComicSansMS.h>
extern ST7735_t3 tft;

#else

#include "TFT.h"
extern TFT tft;

#endif
/*
    initialMode,
    channelMode,
    positionReportMode,
    invalidGpsMode,
*/

Display::Display()
   : _currentMode(Mode::initialMode)
   , _currentFrequency(0)
   {
  strcpy(_currentChannel,"");
  strcpy( _currentRange,"");
  strcpy( _currentTrack,"");
  
}

void Display::formatDist(char* buff, float dist){
  int d10 = int(dist * 10 + 0.5); // to 10ths of NM

   // DDD.D
  int idx = 5;
  if(d10 <= 9999) {
    buff[idx--] = 0;
    buff[idx--] = '0' + (d10 % 10);
    d10 /= 10;
    buff[idx--] = '.';
    buff[idx--] = '0' + (d10 % 10);
    d10 /= 10;
    buff[idx--] = (d10 > 0) ? ('0' + (d10 % 10)) : ' ';
    d10 /= 10;
    buff[idx--] = (d10 > 0) ? ('0' + (d10 % 10)) : ' ';
  } else {
    strcpy(buff,"---.-");
  }
}

void Display::formatBearing(char* buff, float degrees){
  int trk = int(degrees + 0.5);

  int idx = 3;
  buff[idx--] = 0;
  // 3 digit heading
  for(int i=0; i<3; ++i) {
    buff[idx--] = '0' + (trk % 10);
    trk /= 10;
  }
}

void Display::clear(){
  tft.fillScreen(ST7735_BLACK);
  tft.drawRect(0,0,tft.width(), tft.height(), ST7735_BLACK);
  _currentMode = Mode::initialMode; // Nothing drawn yet.
}


// Helper method to see if the channel name has changed.
bool Display::nameChanged(const char* name){
  return strncmp(_currentChannel, name, sizeof(_currentChannel)) != 0;
}


void Display::drawChannel(const char* name, long frequency){

  
  const int BUFFER_SIZE = 64;
  char nameBuff[BUFFER_SIZE];
  int count = 0;
  char* dest = nameBuff;
  while(name[count] && count < (BUFFER_SIZE-1)){
    *dest = name[count];
    ++count;
    ++dest;
  }
  *dest = '\0';


  char freqBuff[16];
  long f = frequency;
  freqBuff[7] = '\0';
  freqBuff[6] = '0' + (f % 10);
  f /= 10;
  freqBuff[5] = '0' + (f % 10);
  f /= 10;
  freqBuff[4] = '0' + (f % 10);
  f /= 10;
  freqBuff[3] ='.';
  freqBuff[2] = '0' + (f % 10);
  f /= 10;
  freqBuff[1] = '0' + (f % 10);
  f /= 10;
  freqBuff[0] = '0' + (f % 10);

  if(_currentMode != Mode::channelMode || _currentFrequency != frequency || nameChanged(name)) {
    clear();
    
    tft.setTextColor(ST7735_WHITE);
  
    tft.setFont(Arial_18);
    tft.setCursor(1,6);
    tft.print(nameBuff);
  
    tft.setFont(Arial_24);
    tft.setCursor(20,65);
    tft.print(freqBuff);
    
    // Keep track of what's drawn.
    _currentFrequency = frequency;
    strncpy(_currentChannel, name, sizeof(_currentChannel));
    strncpy(_currentRange, "", sizeof(_currentRange));
    strncpy(_currentTrack, "", sizeof(_currentTrack));
 
    _currentMode = Mode::channelMode;
  }
}


void Display::updateNavInfo(float nm, float track, bool useKm){

  const int height = 22;
  int y = tft.height() - height;


  if(useKm){
    nm *= 1.852;   // NM to km
  }
  
  // 999.9uu 000

  char trackStr[4];
  formatBearing(trackStr, track);

  const char* unitsStr = (useKm) ? "km" : "NM";

  // DDD.D
  char distStr[6];
  formatDist(distStr, nm);

  // Has the information to display actually changed?
  bool hasChanged =   strncmp(_currentRange, distStr, sizeof(_currentRange)) != 0 
                  ||  strncmp(_currentTrack, trackStr, sizeof(_currentTrack)) != 0;

  // Only update the display if it has changed.                
  if(hasChanged) {  

    tft.fillRect(0, y, tft.width(), height, ST7735_BLACK);
 
    tft.setCursor(1,y);
    tft.setFont(Arial_18);
    tft.print(distStr);
  
    tft.setCursor(tft.getCursorX(), y+6); // +6 is difference in font sizes.
    tft.setFont(Arial_12);
    tft.print(unitsStr);
  
    tft.setFont(Arial_18);
    int16_t x1,y1;
    uint16_t w,h;
    tft.getTextBounds("000",0,0,&x1,&y1,&w,&h);
    tft.setCursor(tft.width() - w, y);
    tft.print(trackStr);

    // Update what has changed.
    strncpy(_currentRange, distStr, sizeof(_currentRange));
    strncpy(_currentTrack, trackStr, sizeof(_currentTrack));
  }
}


// Call this instead of updating the nav information if
// GPS is now invalid.
void Display::invalidGPS(){

  const int height = 22;
  int y = tft.height() - height;

  tft.setTextColor(ST7735_RED);

  tft.setCursor(1,y);
  tft.setFont(Arial_18);
  tft.print("---.-");

  tft.setFont(Arial_18);
  int16_t x1,y1;
  uint16_t w,h;
  tft.getTextBounds("000",0,0,&x1,&y1,&w,&h);
  tft.setCursor(tft.width() - w, y);
  tft.print("---");

  // Update what's displayed to make sure it will
  // redisplay if GPS comes back.
  strncpy(_currentRange, "---", sizeof(_currentRange));
  strncpy(_currentTrack, "---", sizeof(_currentTrack));

}

// Big NO GPS message for startup.
void Display::noGPSReceived(){
  clear();
  
  tft.setTextColor(ST7735_RED);

  tft.setFont(Arial_24);
  tft.setCursor(20,60);
  tft.print("NO GPS");
  _currentMode = Mode::invalidGpsMode;
}

void Display::showPositionReport(float rangeNM, float bearing, const char* cardinalPoint) {

  char trackStr[4];
  formatBearing(trackStr,bearing);

  // DDD.D
  char distStr[6];
  formatDist(distStr, rangeNM);
  char*dist = distStr;
  while(*dist == ' '){ // skip leading spaces here
    ++dist;
  }
  
  clear();
  
  tft.setCursor(1,6);
  tft.setFont(Arial_24);
  tft.print(dist);
  tft.print(" NM");
  
  tft.setCursor(1, 64-12); // centred in Y
  tft.print(cardinalPoint);

  tft.setCursor(1, tft.height() - 30);
  tft.print("Brg: ");
  tft.print(trackStr);

  _currentMode = Mode::positionReportMode;
  
}
