#pragma once

class Settings {
public:
  int gps1bps;
  int gps2bps;
  bool useKm;
  Settings() : gps1bps(9600), gps2bps(9600), useKm(false) {}
};
