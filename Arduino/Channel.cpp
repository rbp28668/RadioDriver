#include <Arduino.h>
#include "Channel.h"

extern const Channel channels[];

Channel::Channel(const char* name, long freq)
: _name(name)
, _freq(freq)
{
}

Channels::Channels()
: _total(0)
{
}

void Channels::init() {
  if(_total == 0) {
    while(channels[_total]._name) {
      ++_total;
    }
  }
}

const Channel* Channels::get(int idx) const {
  return &(channels[idx]);
}

int Channels::channelCount() const {
  return _total;
}
