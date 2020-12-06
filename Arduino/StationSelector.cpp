#include "StationSelector.h"
#include "RadioStation.h"
#include "Selector.h"
#include "devAR620x.h"


extern AR620x radio;

StationSelector::StationSelector()
 
 : channelSelector(&channels)
 , allStationsSelector(&radioStations)
 , nearestStationSelector(&radioStations)
 , _currentLat(0)
 , _currentLon(0)
 , _gpsSetMillis(0)
 , _gpsValid(false)
 , _rtDisplayStart(0)
 , _rtDisplayed(false)
 , _current(0)
 , _useKm(false)

 {
}

void StationSelector::init() {
  channels.init();
  radioStations.init();
  pulse();

}


void StationSelector::pulse() {
  
  
  Selector* prev = _current;
  if(_swA->active()){
    _current = &channelSelector;
  } else if(_swB->active()){
    _current = &allStationsSelector;
  } else {
    _current = &nearestStationSelector;
  }
  bool selectorChanged = _current != prev;
  

  bool gpsStatusChanged = false;
  if( (millis() - _gpsSetMillis) > 1000 * 30) {
    channelSelector.invalidateGPS();
    allStationsSelector.invalidateGPS();
    nearestStationSelector.invalidateGPS();
    gpsStatusChanged = _gpsValid; 
    _gpsValid = false;
  }

  // Pressed button for Nav display?
  if( _navSwitch->active() && _current != &channelSelector){
    if(!_rtDisplayed){
      const RadioStation* station = static_cast<const RadioStation*>(_current->getChannel());
      rtDisplay.set(station, _currentLat, _currentLon);
      rtDisplay.show(&_display);
      _rtDisplayed = true;
    }
    _rtDisplayStart = millis(); // time from button release
  }

  // Nav display on and time expired?
  if( !_navSwitch->active() && _rtDisplayed) {
    if( (millis() - _rtDisplayStart) > 5000) {
      _rtDisplayed = false;
      selectorChanged = true; // force redraw
    }
  }

  if(!_rtDisplayed) {
    if(selectorChanged || gpsStatusChanged) {
      _current->drawCurrent(&_display);
      _current->drawNavigation(&_display, _useKm);
    }
  }
}

void StationSelector::setPosition(float latitude, float longitude, unsigned long age){
  _currentLat = latitude;
  _currentLon = longitude;
  _gpsSetMillis = millis() - age;  
  _gpsValid = true;
  
  // Update all the selectors so they all stay in sync with position.
  // Ensures that Nearest is ready even if not selected.
  channelSelector.setPosition(latitude, longitude, age);
  allStationsSelector.setPosition(latitude, longitude, age);
  nearestStationSelector.setPosition(latitude, longitude, age);

  if(_current) {
    _current->drawCurrent(&_display);
    _current->drawNavigation(&_display, _useKm);
  }
}

void StationSelector::setEncoder(Encoder* encoder, Switch* setSwitch){
  _encoder = encoder;
  _setSwitch = setSwitch;
  encoder->connect(this);
  setSwitch->connect(this);
}

void StationSelector::setSelectionSwitches(Switch* swA, Switch* swB){
  _swA = swA;
  _swB = swB;
}

void StationSelector::setNavSwitch(Switch* navSwitch){
  _navSwitch = navSwitch;
}


void StationSelector::onUp(){
  if(_current) {
    _current->up();
    _current->drawCurrent(&_display);
    _current->drawNavigation(&_display, _useKm);
  }
}

void StationSelector::onDown(){
  if(_current) {
    _current->down();
    _current->drawCurrent(&_display);
    _current->drawNavigation(&_display, _useKm);
  }
}

void StationSelector::onPressed(){
}

void StationSelector::onReleased(){
  if(_current) {
    const Channel* channel = _current->getChannel();
    if(channel) {
      radio.putFreqStandby(channel->_freq, channel->_name);
    }
  }
}
