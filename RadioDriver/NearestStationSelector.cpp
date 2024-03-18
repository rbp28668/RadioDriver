#include <Arduino.h>
#include <limits>
#include "math.h"
#include "NearestStationSelector.h"
#include "RadioStation.h"
#include "Display.h"


// Get nearest station that has distance^2 > nm2 i.e. the closest outside a given radius from 
// this station.
// Also returns distance^2 of nearest in *distance2.
// Call this repeatedly, updating nm2 with the value returned by previous distance2 
const RadioStation* NearestStationSelector::nearestAfter(float nm2, float* distance2){
  //Serial.print("Looking for nearest after ");
  //Serial.println(sqrt(nm2));
  const RadioStation* nearest = 0;
  float minDist = std::numeric_limits<float>::max();
  for(int i=0; i<_stations->stationCount(); ++i){
    float rangeSquared = squaredRangeTo(_stations->get(i));
    if(rangeSquared > nm2 && rangeSquared < minDist){
      nearest = _stations->get(i);
      minDist = rangeSquared;
    }
  }
  *distance2 = minDist;
  return nearest;
}

// Initialises the nearest array with the nearest stations to
// the current latitude and longitude.
void NearestStationSelector::setNearestStations(){
  float distance = 0;
  for(int i=0; i<NEAREST; ++i) {
    nearestStations[i].station = nearestAfter(distance, &distance);
    nearestStations[i].squaredRange = distance;
  }
}

// Recalculates all the ranges based on new current position.
void NearestStationSelector::recalculateRanges(){
  for(int i=0; i<NEAREST; ++i) {
    if(nearestStations[i].station == 0) {
      break;
    }
    nearestStations[i].squaredRange = squaredRangeTo(nearestStations[i].station);
  }
}

// Effectively does a single pass of bubble-sort exchanging any neighbours that are
// out of sequence.  Assumption is that the array is at worst almost sorted and any
// out of sequence items will soon end up sorted again.
void NearestStationSelector::reSortStations() {
  for(int i=0; i<NEAREST-1; ++i) {
    if(nearestStations[i+1].station == 0) {
      break;
    }

    if(nearestStations[i].squaredRange > nearestStations[i+1].squaredRange){
      StationRange temp = nearestStations[i];
      nearestStations[i] = nearestStations[i+1];
      nearestStations[i+i] = temp;
    }
  }
}

// Replaces the furthest away station with the closest further away than the 
// second further away.  May not change as more often than not it will just
// find the same station.  Once a station is put in the top then it will
// find its position after successive calls to reSortStations.
void NearestStationSelector::replaceFurthest(){
  int top = NEAREST-1;
  while(top > 0 && nearestStations[top].station == 0) {
    --top;
  }
  float dist = nearestStations[top-1].squaredRange;
  const RadioStation* other = nearestAfter(dist, &dist);
  nearestStations[top].station = other;
  nearestStations[top].squaredRange = dist;
}



void NearestStationSelector::setPosition(float latitude, float longitude, unsigned long age){

  RadioStationSelector::setPosition(latitude, longitude);
  if(hasInitialPosition) {
    recalculateRanges();
    reSortStations();
    replaceFurthest();
  } else {  // Initialise from initial position
    setNearestStations();
    hasInitialPosition = true;
    _index = 0;
  }
}

NearestStationSelector::NearestStationSelector(RadioStations* stations) :
  _stations(stations)
  , _index(0)
  , hasInitialPosition(false)
{
  // NOP  
}

void NearestStationSelector::up() {
  if(hasInitialPosition) {
    int top = NEAREST-1;
    while(top > 0 && nearestStations[top].station == 0) {
      --top;
    }

    ++_index;
    if(_index >top) {
      _index = 0;
    }
  } 
}

void NearestStationSelector::down() {
  if(hasInitialPosition) {
    --_index;
    if(_index < 0) {
      int top = NEAREST-1;
      while(top > 0 && nearestStations[top].station == 0) {
        --top;
      }
      _index = top;
    }
  }
}

void NearestStationSelector::drawCurrent(Display* display) {
  if(hasInitialPosition) {
    RadioStationSelector::drawCurrent(display);
  } else {
    display->noGPSReceived();
  }
}


void NearestStationSelector::drawNavigation(Display* display, bool useKm) {
  if(hasInitialPosition){
    if(isPositionValid()) {
      float nm;
      float track;
      navToStation(nearestStations[_index].station,&nm,&track);
      display->updateNavInfo(nm,track, useKm);
    }else{
      display->invalidGPS();
    } 
  }
}

const Channel* NearestStationSelector::getChannel(){
  if(hasInitialPosition) {
    return nearestStations[_index].station;
  }
  return 0;
}
  
