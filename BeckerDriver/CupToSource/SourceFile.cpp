#include <string>
#include "SourceFile.h"
#include "RadioStation.h"

SourceFile::SourceFile(const char* name) 
: out(name)
, isOpen(false)
{


}
SourceFile::~SourceFile()
{
	if (isOpen) {
		finish();
	}
	out.close();
}

int SourceFile::convertFrequency(float freq) {
	return int(freq * 1000 + 0.5);
}

void SourceFile::start()
{
	out << "#include \"RadioStation.h\"" << std::endl;
	out << std::endl;
	out << "extern const RadioStation stations[] = {" << std::endl;
	isOpen = true;
}

void SourceFile::add(const RadioStation& station)
{
	out << "{\"" << station.name << '"'
		<< "," << convertFrequency(station.frequency)
		<< "," << station.lat
		<< "," << station.lon
		<< "}," << std::endl;

}

void SourceFile::finish()
{
	out << "{0, 0, 0, 0L}" << std::endl;
	out << "};";
	isOpen = false;
}


