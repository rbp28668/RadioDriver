#include <math.h>
#include "RadioStation.h"
#include "RadioStationSelector.h"
#include "Display.h"

RadioStationSelector::RadioStationSelector()
  : currentLat(0)
  , currentLon(0)
  , distPerDegreeLon(2*pi*R/360)
  , positionValid(false)
{
}

void RadioStationSelector::setPosition(float latitude, float longitude){
  currentLat = latitude;
  currentLon = longitude;

  // Assumes a rectangular grid centred on the current position. 
  distPerDegreeLon = 2 * pi * R * cos(latitude * pi / 180) / 360;
  positionValid = true;
}

void RadioStationSelector::invalidateGPS(){
  positionValid = false;
}

float RadioStationSelector::squaredRangeTo(const RadioStation* station) {
  float dy = (station->_lat - currentLat) * distPerDegreeLat;
  float dx = (station->_lon - currentLon) * distPerDegreeLon;
  return dx * dx + dy * dy;    
}


void RadioStationSelector::navToStation(const RadioStation* station, float* rangeNM, float* track) {

  float dy = (station->_lat - currentLat) * distPerDegreeLat;
  float dx = (station->_lon - currentLon) * distPerDegreeLon;
  *rangeNM = sqrt( dx * dx + dy * dy );   
  float trackRadians = pi/2 - atan2(dy, dx); // in radians with rotation to align to 0 being north
  if (trackRadians < 0) {
    trackRadians += 2 * pi;
  }
  *track = trackRadians * 180 / pi;
}

void RadioStationSelector::drawCurrent(Display* display){
  const Channel* channel = getChannel();
  if(channel){
    display->clear();
    display->drawChannel(channel->_name, channel->_freq);  
  }
}
