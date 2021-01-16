#include "AllStationsSelector.h"
#include "Display.h"
#include "StationSelector.h"
#include "RadioStation.h"

AllStationSelector::AllStationSelector(RadioStations* stations)
: _stations(stations)
, _index(0)
{
  
}

void AllStationSelector::up(){
  ++_index;  
  if(_index >= _stations->stationCount()) {
    _index = 0;
  }
}

void AllStationSelector::down(){
  --_index;  
  if(_index < 0){
    _index = _stations->stationCount()-1;
  }
}


void AllStationSelector::drawNavigation(Display* display, bool useKm){
  if(isPositionValid()) {
    float nm;
    float track;
    navToStation(_stations->get(_index), &nm, &track);
    display->updateNavInfo(nm, track, useKm);
  } else {
    display->invalidGPS();
  }
}

void AllStationSelector::setPosition(float latitude, float longitude, unsigned long age){
  RadioStationSelector::setPosition(latitude, longitude);
}

const Channel* AllStationSelector::getChannel(){
  return _stations->get(_index);
}
