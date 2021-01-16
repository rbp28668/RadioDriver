#pragma once
#include <fstream>

class RadioStation;

class SourceFile
{
	std::ofstream out;
	bool isOpen;

	int convertFrequency(float freq);

public:
	SourceFile(const char* name);
	~SourceFile();
	void start();
	void add(const RadioStation& station);
	void finish();
};

