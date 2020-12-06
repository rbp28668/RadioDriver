#ifndef DEV_AR620x_H
#define DEV_AR620x_H

#include <Arduino.h>
#include "BasicSerial.h"

#define NAME_SIZE 64

// Basically a fixed point number - frequency x1000 so 120.965 is 120965
typedef long FrequencyT;

typedef struct _Radio_t
{
	FrequencyT ActiveFrequency;    //active station frequency
	FrequencyT PassiveFrequency;   // passive (or standby) station frequency
	char PassiveName[NAME_SIZE];    // passive (or standby) station name
	char ActiveName[NAME_SIZE];     //active station name
	int Volume;               // Radio Volume
	int Squelch;              // Radio Squelch
	int Vox;                  // Radio Intercom Volume
	bool Changed;              // Parameter Changed Flag            (TRUE = parameter changed)
	bool Enabled;              // Radio Installed d Flag            (TRUE = Radio found)
	bool Dual;                 // Dual Channel mode active flag     (TRUE = on)
	bool Enabled8_33;          // 8,33kHz Radio enabled             (TRUE = 8,33kHz)
	bool RX;                   // Radio reception active            (TRUE = reception)
	bool TX;                   // Radio transmission active         (TRUE = transmission)
	bool RX_active;            // Radio reception on active station (TRUE = reception)
	bool RX_standy;            // Radio reception on passive        (standby) station
	bool lowBAT;               // Battery low flag                  (TRUE = Batt low)
	bool TXtimeout;            // Timeout while transmission (2Min)
	float BattVoltage; 
}Radio_t;


class AR620x {
    BasicSerial* serial;
    int sendCommand(uint8_t szTmp[], uint16_t len);
    int  setStation(uint8_t* Command, int Active_Passive, FrequencyT fFrequency, const char* Station);
    FrequencyT idx2Freq(uint16_t uFreqIdx);
    uint16_t Frq2Idx(FrequencyT fFreq);
	int convertAnswer(uint8_t* szCommand, int len, uint16_t CRC);

	Radio_t radioState;

public:
    AR620x(BasicSerial* pbs);
    bool putVolume(int Volume);
    bool putSquelch(int Squelch);
    bool putFreqActive(FrequencyT Freq, const char StationName[]);
    bool putFreqStandby(FrequencyT Freq, const char StationName[]);
    bool stationSwap();
    bool radioMode(int mode);

    const Radio_t& state() { return radioState; }
    
	bool parseString(char* String, int len);

};


#endif
