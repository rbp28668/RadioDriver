#pragma once

#include "Channel.h"

class RadioStation : public Channel {
  public:
  float _lat;
  float _lon;

  RadioStation(const char* name, long freq, float latitude, float longitude);
};

class RadioStations {
  int _total;

public:
  RadioStations();
  void init();
  const RadioStation* get(int idx) const;
  int stationCount() const;

};
