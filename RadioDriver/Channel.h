#pragma once

// Class to hold channels "with no fixed abode" i.e. there is no lat/long
// E.g. gliding air to air,  D&D.
// Derived class RadioStation adds position for fixed stations.

class Channel {
  public:
  char _name[16];
  long _freq; // in kHz - taken channel number as written and multiply by 1000;
  Channel(const char* name, long freq);
};

class Channels {
  const static int MAX_CHANNELS = 32;
  const Channel* _channels[MAX_CHANNELS];
  int _total;

public:
  Channels();
  ~Channels();
  void loadDefaults();
  void add(const Channel* channel);
  const Channel* get(int idx) const;
  int channelCount() const;

};
