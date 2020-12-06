
#pragma once
class RadioStation;
class Display;

class RTDisplay {
  const static char* cardinalPoints[];
  const float R = 6371 / 1.852; // radius of earth in NM (convert km to NM)
  const float pi = 3.1415926535897932384636434;
  float _rangeNM;
  float _track;
  const char* _cardinalPoint;
 
public:
  RTDisplay();
  void set(const RadioStation*, float lat, float lon);
  void show(Display* display);
};
