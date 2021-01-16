#pragma once

#include "Channel.h"

class RadioStation : public Channel {
  public:
  float _lat;
  float _lon;

  RadioStation(const char* name, long freq, float latitude, float longitude);
};

class RadioStations {
  const static int MAX_STATIONS = 1024;
  const RadioStation* _stations[MAX_STATIONS];
  int _total;

public:
  RadioStations();
  ~RadioStations();
  void init();
  void loadDefaults();
  void add(const RadioStation* station);
  void sortByName();
  const RadioStation* get(int idx) const;
  int stationCount() const;

};
