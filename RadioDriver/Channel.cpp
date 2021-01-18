#include <Arduino.h>
#include "Channel.h"

extern const Channel defaultChannels[];

Channel::Channel(const char* name, long freq)
: _freq(freq)
{
  if(name && freq) {
    strncpy(_name, name, sizeof(_name));
    _name[sizeof(_name) -1] = 0;
  } else {
    _name[0] = 0;
  }
}

Channels::Channels()
: _total(0)
{
}

Channels::~Channels(){
  for(int i=0; i<_total; ++i){
    delete _channels[i];
  }
}

void Channels::loadDefaults() {
  if(_total == 0) {
    int idx = 0;
    while(defaultChannels[_total]._freq != 0) {
      add(new Channel(defaultChannels[idx]._name, defaultChannels[idx]._freq));
      ++idx;
    }
  }
}

void Channels::add(const Channel* channel){
  if(_total < MAX_CHANNELS){  
    _channels[_total] = channel;
    ++_total;
  }
}

const Channel* Channels::get(int idx) const {
  if(idx < 0) idx = 0;
  if(idx >= _total) idx = _total-1;
  return _channels[idx];
}

int Channels::channelCount() const {
  return _total;
}
