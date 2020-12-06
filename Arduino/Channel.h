#pragma once

// Class to hold channels "with no fixed abode" i.e. there is no lat/long
// E.g. gliding air to air,  D&D.
// Derived class RadioStation adds position for fixed stations.

class Channel {
  public:
  const char* _name;
  long _freq; // in kHz - taken channel number as written and multiply by 1000;
  Channel(const char* name, long freq);
};

class Channels {
  int _total;

public:
  Channels();
  void init();
  const Channel* get(int idx) const;
  int channelCount() const;

};
