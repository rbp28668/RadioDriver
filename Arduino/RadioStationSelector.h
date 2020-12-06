#pragma once
#include "Selector.h"

class RadioStation;
class Display;

class RadioStationSelector : public Selector{

  float currentLat;
  float currentLon;
  bool positionValid;
  const float R = 6371 / 1.852; // radius of earth in NM (convert km to NM)
  const float pi = 3.1415926535897932384636434;
  const float distPerDegreeLat = 2 * pi * R / 360; // constant 
  float distPerDegreeLon; // varies by latitude

protected:
  bool isPositionValid() const { return positionValid; }
  
public:
  RadioStationSelector();
  float squaredRangeTo(const RadioStation* station);
  void navToStation(const RadioStation* station, float* rangeNM, float* track);
  void setPosition(float latitude, float longitude);

  virtual void up() = 0;
  virtual void down() = 0;
  virtual void drawCurrent(Display* display);
  virtual void drawNavigation(Display* display, bool useKm) = 0;
  virtual void setPosition(float latitude, float longitude, unsigned long age) = 0;
  virtual void invalidateGPS();
  virtual const Channel* getChannel() = 0;

};
