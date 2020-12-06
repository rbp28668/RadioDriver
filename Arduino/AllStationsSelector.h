#pragma once

#include "Selector.h"
#include "RadioStationSelector.h"

class RadioStations;


class AllStationSelector :  public RadioStationSelector {
  const RadioStations* _stations;
  int _index;
 
public:

  AllStationSelector(RadioStations* stations);  
  virtual void up();
  virtual void down();
  virtual void drawNavigation(Display* display, bool useKm);
  virtual void setPosition(float latitude, float longitude, unsigned long age);
  virtual const Channel* getChannel();

};
