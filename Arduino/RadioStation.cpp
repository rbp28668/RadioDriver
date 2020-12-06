#include <Arduino.h>
#include "RadioStation.h"

const extern RadioStation stations[];

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

void RadioStations::init() {
  if(_total == 0) {
    while(stations[_total]._name) {
      ++_total;
    }
  }
}

const RadioStation* RadioStations::get(int idx) const{
  if(idx < 0) idx = 0;
  if(idx >= _total) idx = _total-1;
  return stations + idx;  
}

int RadioStations::stationCount() const {
  return _total;
}
