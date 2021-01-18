#pragma once
#include "encoder.h"
#include "Switch.h"
#include "Display.h"
#include "RadioStation.h"
#include "Channel.h"

#include "ChannelSelector.h"
#include "AllStationsSelector.h"
#include "NearestStationSelector.h"
#include "RTDisplay.h"

class RadioStation;
class Selector;

class StationSelector : public Encoder::Receiver, public Switch::Receiver {

  Channels* channels;
  RadioStations* radioStations;

  ChannelSelector channelSelector;
  AllStationSelector allStationsSelector;
  NearestStationSelector nearestStationSelector;
  RTDisplay rtDisplay;

  // Track current position.
  float _currentLat;
  float _currentLon;
  unsigned long _gpsSetMillis; 
  bool _gpsValid;

  unsigned long _rtDisplayStart;
  bool _rtDisplayed;
  
  Selector* _current;  
  Display _display;
  bool _useKm;  

  Encoder* _encoder;
  Switch* _setSwitch;
  Switch* _swA;
  Switch* _swB;
  Switch* _navSwitch;
  
public:
  StationSelector(RadioStations* radioStations, Channels* channels);

  void useKm(bool km = true) {_useKm = km;}
  void init();
  void pulse();
  void setPosition(float latitude, float longitude, unsigned long age);

  void setEncoder(Encoder* encoder, Switch* setSwitch);
  void setSelectionSwitches(Switch* swA, Switch* swB);
  void setNavSwitch(Switch* navSwitch);
  
  // Encoder
  virtual void onUp();
  virtual void onDown();

  // switch
  virtual void onPressed(); 
  virtual void onReleased();
};
