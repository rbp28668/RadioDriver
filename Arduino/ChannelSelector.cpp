#include "ChannelSelector.h"
#include "Channel.h"
#include "Display.h"

ChannelSelector::ChannelSelector(Channels* channels)
: _index(0)
, _channels(channels)

{
  
}

void ChannelSelector::up() {
  ++_index;
  if(_index >= _channels->channelCount()) {
    _index = 0;  
  }
}

void ChannelSelector::down(){
  --_index;
  if(_index < 0) {
    _index = _channels->channelCount()-1;
  }
}

void ChannelSelector::drawCurrent(Display* display) {
  const Channel* channel = _channels->get(_index);
  display->drawChannel(channel->_name, channel->_freq);
}

void ChannelSelector::drawNavigation(Display* display, bool useKm){
  // Nop as channels have no location.
}

void ChannelSelector::setPosition(float latitude, float longitude, unsigned long age){
  // Nop
}

void ChannelSelector::invalidateGPS(){
  // Nop
}

const Channel* ChannelSelector::getChannel() {
  return _channels->get(_index);
}
