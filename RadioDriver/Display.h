#pragma once

// Display
// Manages the TFT display.
// Note that this now tracks what's displayed and only updates when necessary.
// Don't use clear() indiscriminately as it will then cause screen flicker as
// the rest of the screen then needs to update.

class Display {

  // Different types of display mode.
  enum Mode {
    initialMode,       // initial mode to ensure screen redrawn
    channelMode,       // channel (optionally with Nav info)
    positionReportMode, // Position report text
    invalidGpsMode,    // No GPS display
  };

  // Track what's displayed to see if we actually need to redraw.
  Mode _currentMode;
  char _currentChannel[16];
  long _currentFrequency;
  char _currentRange[16];
  char _currentTrack[16];

  bool nameChanged(const char* name);
  void formatDist(char* buff, float dist);
  void formatBearing(char* buff, float degrees);
public:
   Display();
   void clear();
   void drawChannel(const char* name, long frequency);
   void updateNavInfo(float nm, float track, bool useKm);
   void noGPSReceived();
   void invalidGPS();
   void showPositionReport(float rangeNM, float bearing, const char* cardinalPoint);
};
