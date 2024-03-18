// CupToSource.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "CupFile.h"
#include "SourceFile.h"


int main()
{
	CupFile inputData;
	//std::ifstream airfields("E:\\Git\\RadioDriver\\Data\\UK AF and OL 2020-10-31.cup");
	//std::ifstream turnpoints("E:\\Git\\RadioDriver\\Data\\BGA TPs 2020-10-31.cup");
	std::ifstream combined24("D:\\Projects\\RadioDriver\\Data\\Combined 2024-03-01.cup");
	
	//inputData.read(airfields);
	//inputData.read(turnpoints);
	inputData.read(combined24);

	inputData.sortByName();

	SourceFile output("Stations.cpp");
	output.start();
	for (auto iter = inputData.begin(); iter != inputData.end(); ++iter) {
		output.add(*(iter->get()));
	}
	output.finish();
    
}

