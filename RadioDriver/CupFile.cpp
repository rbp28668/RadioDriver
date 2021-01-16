#include <cstring>
#include "CupFile.h"
#include "RadioStation.h"

#include "SdFat.h"

float stof(const char* str, size_t* pos = 0) {

  char* endp = 0;
  double val = strtod(str, &endp);
  if(pos){
    *pos = endp - str;
  }
  return val;
}

// Splits input buffer up one token at a time.
const char* CupFile::next(char* is, size_t& pos) {
	while (isspace(is[pos])) {
		++pos;
	}

	bool hasQuotes = is[pos] == '"';
	char term = (hasQuotes) ? '"' : ',';

	if (hasQuotes) {
		++pos;
	}

	const char* value = is + pos;
	while (is[pos] && is[pos] != term) {
		++pos;
	}
  // have found trailing null, ' or comma so terminate section here.
  is[pos] = 0;

	if (hasQuotes) {
		++pos;  // trailing quote
	}

	while (isspace(is[pos])) {
		++pos;
	}

	++pos; // skip comma

	return value;
}

long CupFile::convertFrequency(const char* freq) {
	long f =  (long) (1000 * strtod(freq, 0) + 0.5);
  return f;
}


// Convert latitude - first 2 digits are 00 to 90, then decimal minutes followed by N/S selector
// E.g. 5257.733N  52 degrees, 27.733mins north
float CupFile::convertLat(const char* latitude) {

  // split off first 2 digits as degrees
	char degrees[3];
  strncpy(degrees, latitude, 2);
  degrees[2] = 0;

  // Rest is mins and N/S indicator
	const char* mins = latitude + 2;

	size_t idx;
	float val = stof(degrees);
	val += stof(mins, &idx) / 60;

	char NS = mins[idx];
	if (NS == 'S' || NS == 's') {
		val = 0 - val;
	}
	return val;
}


// Convert longitude - first 3 digits are 000 to 180, then decimal minutes followed by E/W selector
// E.g.00223.383W is 2 degrees and 23.383 minutes west
float CupFile::convertLon(const char* longitude) {
	char degrees[4];
	strncpy(degrees, longitude, 3);
  degrees[3] = 0;
	const char* mins = longitude + 3;

	size_t idx;
	float val = stof(degrees);
	val += stof(mins, &idx) / 60;

	char EW = mins[idx];
	if (EW == 'W' || EW == 'w') {
		val = 0 - val;
	}
	return val;
}


void CupFile::read(FsFile& file, RadioStations& stations)
{
  char line[512];  // in practice lines are up to about 350 chars.
  file.fgets(line, sizeof(line));
  while (file.fgets(line, sizeof(line)) > 0) {
    size_t pos = 0;
		const char* name = next(line, pos);
		/*const char* code =*/ next(line, pos);
		/*const char* country =*/ next(line, pos);
		const char* lat = next(line, pos);
		const char* lon = next(line, pos);
		/*const char* elev =*/ next(line, pos);
		/*const char* style =*/ next(line, pos);
		/*const char* rwdir =*/ next(line, pos);
		/*const char* rwlen =*/ next(line, pos);
		const char* freq = next(line, pos);
		/*const char* desc =*/ next(line, pos);
		/*const char* userdata =*/ next(line, pos);
		/*const char* pics =*/ next(line, pos);

    long f = convertFrequency(freq);
		if (f > 0) {
			RadioStation* station = new RadioStation(name, f, convertLat(lat), convertLon(lon));
			stations.add(station);
		}
	}
}

void CupFile::read(FsFile& file, Channels& channels){
  char line[512];  // probably smaller but not a problem
  file.fgets(line, sizeof(line));
  while (file.fgets(line, sizeof(line)) > 0) {
    size_t pos = 0;
    const char* name = next(line, pos);
    const char* freq = next(line, pos);
    long f = convertFrequency(freq);
    if (f > 0) {
      Channel* channel = new Channel(name, f);
      channels.add(channel);
    }
  }
}

void CupFile::read(FsFile& file, Settings& settings){
  char line[512];  // probably smaller but not a problem
  size_t maxlen = sizeof(line);
  
   while (file.fgets(line, maxlen) > 0) {
    size_t pos = 0;

    // Skip any comments
    if(line[pos] == '#') continue;
    
    // Skip leading spaces
    while ( pos < maxlen && isspace(line[pos])) {
      ++pos;
    }
    // Start of label
    char* label = line+pos;

    // skip label contents until space or =
    while ( pos < maxlen && !isspace(line[pos]) && line[pos] != '=' ) {
      ++pos;
    }

    if(pos == maxlen) continue; // abort this line, not formatted sensibly

    // Any trailing space(s) look for = sign.
    if(isspace(line[pos])){
      while (pos < maxlen && line[pos] != '=') {
        line[pos] = 0;
        ++pos;
      }
    }
    if(pos == maxlen) continue; // abort this line, not formatted sensibly

    line[pos++] = 0;  // Remove equals sign to ensure label terminated
    
    // Skip leading spaces for value.
    while ( pos < maxlen && isspace(line[pos])) {
      ++pos;
    }

    if(pos == maxlen) continue; // abort this line, not formatted sensibly
    char* value = line+pos;  

    // Should now have 2 strings, label and value 
    if(strcmp(label,"gps1") == 0) {
      settings.gps1bps = atoi(value);
    } else if(strcmp(label,"gps2") == 0) {
      settings.gps2bps= atoi(value);        
    } else if(strcmp(label,"units") == 0) {
      settings.useKm = strcmp(value, "km"); 
    }

  }
}
