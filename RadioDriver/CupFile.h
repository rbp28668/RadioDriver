#pragma once
#include "RadioStation.h"
#include "Settings.h"

class FsFile;

class CupFile
{
	const char* next(char* is, size_t& pos);
	long convertFrequency(const char* freq);
	float convertLon(const char* longitude);
	float convertLat(const char* latitude);
public:
	void read(FsFile& file, RadioStations& stations);
  void read(FsFile& file, Channels& channels);
  void read(FsFile& file, Settings& settings);
};
