#pragma once
#include <string>
#include <istream>
#include <list>
#include <vector>
#include "RadioStation.h"

class CupFile
{
	std::vector<std::unique_ptr<RadioStation>> stations;

	std::string next(std::istream& is);
	float convertFrequency(const std::string& freq);
	float convertLon(const std::string& longitude);
	float convertLat(const std::string& latitude);
public:
	void read(std::istream& is);
	void sortByName();
	std::vector<std::unique_ptr<RadioStation>>::iterator begin() { return stations.begin(); }
	std::vector<std::unique_ptr<RadioStation>>::iterator end() { return stations.end(); }
};

