#pragma once

#include "devAR620x.h"

class Settings {
public:
  int gps1bps;
  int gps2bps;
  bool useKm;

  // Default radio frequencies as per devAR620x.h
  FrequencyT ActiveFrequency;   //active station frequency
  FrequencyT StandbyFrequency;  // passive (or standby) station frequency
  char ActiveName[NAME_SIZE];   //active station name
  char StandbyName[NAME_SIZE];  // passive (or standby) station name

  Settings()
    : gps1bps(9600), gps2bps(9600), useKm(false), ActiveFrequency(0), StandbyFrequency(0) {
    StandbyName[0] = 0;
    ActiveName[0] = 0;
  }
};
