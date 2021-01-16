#pragma once

class Channel;
class Display;

// Interface that allows you to select a channel or frequency
// from a list

class Selector {
  public:
  virtual void up() = 0;
  virtual void down() = 0;
  virtual void drawCurrent(Display* display)= 0;
  virtual void drawNavigation(Display* display, bool useKm) = 0;
  virtual void setPosition(float latitude, float longitude, unsigned long age) = 0;
  virtual void invalidateGPS() = 0;
  virtual const Channel* getChannel() = 0;
};  
