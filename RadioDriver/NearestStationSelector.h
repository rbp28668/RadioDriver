#include "RadioStationSelector.h"

class RadioStation;
class RadioStations;
class Display;

class NearestStationSelector : public RadioStationSelector {

  struct StationRange {
    const RadioStation* station;
    float squaredRange;  // don't bother with sqrt for most sums.
  };

  // Radio stations to use.
  const RadioStations* _stations;     
   
  const static int NEAREST = 20;
  StationRange nearestStations[NEAREST];

  int _index;
  bool hasInitialPosition; // when initialised by GPS
  

  const RadioStation* nearestAfter(float nm2, float* distance);
  void setNearestStations();
  void recalculateRanges();
  void reSortStations();
  void replaceFurthest();


public:
  NearestStationSelector(RadioStations* stations);
  virtual void up();
  virtual void down();
  virtual void drawCurrent(Display* display);
  virtual void drawNavigation(Display* display, bool useKm);
  virtual void setPosition(float latitude, float longitude, unsigned long age);
  virtual const Channel* getChannel();
 
};
