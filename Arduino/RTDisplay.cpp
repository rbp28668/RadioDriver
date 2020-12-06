
#include <Arduino.h>
#include <math.h>
#include "RTDisplay.h"
#include "Display.h"
#include "RadioStation.h"


const char* RTDisplay::cardinalPoints[] = {
    "N",
    "NNE",
    "NE",
    "ENE",
    "E",
    "ESE",
    "SE",
    "SSE",
    "S",
    "SSW",
    "SW",
    "WSW",
    "W",
    "WNW",
    "NW",
    "NNW"
  };


RTDisplay::RTDisplay() 
: _rangeNM(0)
, _track(0)
, _cardinalPoint("---")
{
}

void RTDisplay::set(const RadioStation* station, float lat, float lon) {


  // Assumes a rectangular grid centred on the current position. 
  float distPerDegreeLat = 2 * pi * R / 360; // constant 
  float distPerDegreeLon = 2 * pi * R * cos(lat * pi / 180) / 360;

  float dy = (lat - station->_lat) * distPerDegreeLat;
  float dx = (lon - station->_lon) * distPerDegreeLon;
  
  _rangeNM = sqrt( dx * dx + dy * dy );   
  
  float trackRadians = pi/2 - atan2(dy, dx); // in radians with rotation to align to 0 being north
  if (trackRadians < 0) {
    trackRadians += 2 * pi;
  }
  _track = trackRadians * 180 / pi;

  int idx = int( (16 * trackRadians) / (2*pi) + 0.5);
  idx &= 15;
  _cardinalPoint = cardinalPoints[idx];
}

void RTDisplay::show(Display* display){
  display->showPositionReport(_rangeNM, _track, _cardinalPoint);
}
