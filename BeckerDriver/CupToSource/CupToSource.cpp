// CupToSource.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

std::string next(std::istream& is) {
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

float convertLat(const std::string& latitude) {

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

float convertLong(const std::string& longitude) {
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

int convertFrequency(const std::string& freq) {
	return int(std::stod(freq) * 1000 + 0.5);
}

int main()
{
	const char* inputFile = "E:\\Users\\rbp28668\\source\\BeckerDriver\\radio.cup";
	std::ifstream ifs(inputFile);

	
	//std::ostream& out = std::cout;
	std::ofstream out("Stations.cpp");
    
	out << "#include \"RadioStation.h\"" << std::endl;
	out << std::endl;
	out << "extern const RadioStation stations[] = {" << std::endl;

	std::string line;
	std::getline(ifs, line); // skip initial line
	while(std::getline(ifs, line)) {
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
			out << "{\"" << name << '"'
				<< "," << convertFrequency(freq)
				<< "," << convertLat(lat)
				<< "," << convertLong(lon)
				<< "}," << std::endl;
		}
	}

	out << "{0, 0, 0, 0L}" << std::endl;
	out << "};";
	
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
