#include <sstream>
#include <algorithm>
#include "CupFile.h"

std::string CupFile::next(std::istream& is) {
	while (isspace(is.peek())) {
		is.get();
	}

	bool hasQuotes = is.peek() == '"';
	char term = (hasQuotes) ? '"' : ',';

	if (hasQuotes) {
		is.get();
	}

	std::string value;
	while (is && is.peek() != term) {
		value.push_back(is.get());
	}

	if (hasQuotes) {
		is.get();
	}

	while (isspace(is.peek())) {
		is.get();
	}

	int comma = is.get();

	return value;
}

float CupFile::convertFrequency(const std::string& freq) {
	return std::stof(freq);
}


float CupFile::convertLat(const std::string& latitude) {

	std::string degrees = latitude.substr(0, 2);
	std::string mins = latitude.substr(2);

	size_t idx;
	float val = std::stof(degrees);
	val += std::stof(mins, &idx) / 60;

	char NS = mins[idx];
	if (NS == 'S' || NS == 's') {
		val = 0 - val;
	}
	return val;
}


float CupFile::convertLon(const std::string& longitude) {
	std::string degrees = longitude.substr(0, 3);
	std::string mins = longitude.substr(3);

	size_t idx;
	float val = std::stof(degrees);
	val += std::stof(mins, &idx) / 60;

	char EW = mins[idx];
	if (EW == 'W' || EW == 'w') {
		val = 0 - val;
	}
	return val;
}


void CupFile::read(std::istream& input)
{
	std::string line;
	std::getline(input, line); // skip initial line
	while (std::getline(input, line)) {
		std::istringstream is(line);

		std::string name = next(is);
		std::string code = next(is);
		std::string country = next(is);
		std::string lat = next(is);
		std::string lon = next(is);
		std::string elev = next(is);
		std::string style = next(is);
		std::string rwdir = next(is);
		std::string rwlen = next(is);
		std::string freq = next(is);
		std::string desc = next(is);
		std::string userdata = next(is);
		std::string pics = next(is);

		if (!freq.empty()) {
			RadioStation* station = new RadioStation();
			station->name = name;
			station->lat = convertLat(lat);
			station->lon = convertLon(lon);
			station->frequency = convertFrequency(freq);

			stations.push_back(std::unique_ptr<RadioStation>(station));
		}
	}
}

bool compare(const std::unique_ptr<RadioStation>& a,
	const std::unique_ptr<RadioStation>& b) {
	return a->name < b->name;
}
void CupFile::sortByName()
{
	std::sort(stations.begin(), stations.end(), compare);
}
