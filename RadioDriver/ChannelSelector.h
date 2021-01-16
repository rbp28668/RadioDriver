#include "Selector.h"

class Channels;
class Display;

class ChannelSelector : public Selector {
  int _index;
  const Channels* _channels;
  
public:

  ChannelSelector(Channels* channels);  
  virtual void up();
  virtual void down();
  virtual void drawCurrent(Display* display);
  virtual void drawNavigation(Display* display, bool useKm);
  virtual void setPosition(float latitude, float longitude, unsigned long age);
    virtual void invalidateGPS();
virtual const Channel* getChannel();

};
