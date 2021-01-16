#include <Arduino.h>
#include "RadioStation.h"

const extern RadioStation defaultStations[];

RadioStation::RadioStation(const char* name, long freq, float latitude, float longitude)
: Channel(name, freq)
{
  _lat = latitude;
  _lon = longitude;
}


RadioStations::RadioStations() 
  : _total(0)
{
}

RadioStations::~RadioStations(){
  for(int i=0; i<_total; ++i){
    delete _stations[i];
  }
}

void RadioStations::init() {
  _total = 0;
}

void RadioStations::loadDefaults(){
  if(_total == 0) {
    int idx = 0;
    while(defaultStations[idx]._freq != 0) {
      add(new RadioStation(defaultStations[idx]._name, defaultStations[idx]._freq, defaultStations[idx]._lat, defaultStations[idx]._lon));
      ++idx;
    }
  }
}


void RadioStations::add(const RadioStation* station){
  if(_total < MAX_STATIONS){  
    _stations[_total] = station;
    ++_total;
  }
}

static int compare(const void* p1, const void* p2){
  RadioStation* r1 = *static_cast< RadioStation* const*>(p1);
  RadioStation* r2 = *static_cast< RadioStation* const*>(p2);
  return strcmp(r1->_name, r2->_name);
}

void RadioStations::sortByName(){
  qsort(_stations, _total, sizeof(RadioStation*), compare);
}

const RadioStation* RadioStations::get(int idx) const{
  if(idx < 0) idx = 0;
  if(idx >= _total) idx = _total-1;
  return _stations[idx];
}

int RadioStations::stationCount() const {
  return _total;
}
