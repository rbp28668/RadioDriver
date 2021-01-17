#pragma once

class Display {
  void formatDist(char* buff, float dist);
  void formatBearing(char* buff, float degrees);
  public:
   void clear();
   void drawChannel(const char* name, long frequency);
   void updateNavInfo(float nm, float track, bool useKm);
   void noGPSReceived();
   void invalidGPS();
   void showPositionReport(float rangeNM, float bearing, const char* cardinalPoint);
};
