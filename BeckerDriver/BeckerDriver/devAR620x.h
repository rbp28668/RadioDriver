#ifndef DEV_AR620x_H
#define DEV_AR620x_H

#include "BasicSerial.h"

// Arduino delay
extern void delay(int mS);


#define NAME_SIZE 64

typedef struct _Radio_t
{
	double ActiveFrequency;    //active station frequency
	double PassiveFrequency;   // passive (or standby) station frequency
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
    int  setStation(uint8_t* Command, int Active_Passive, double fFrequency, const char* Station);
    double idx2Freq(uint16_t uFreqIdx);
	int convertAnswer(uint8_t* szCommand, int len, uint16_t CRC);

	Radio_t radioState;

public:
    AR620x(BasicSerial* pbs);
    bool putVolume(int Volume);
    bool putSquelch(int Squelch);
    bool putFreqActive(double Freq, const char StationName[]);
    bool putFreqStandby(double Freq, const char StationName[]);
    bool stationSwap();
    bool radioMode(int mode);

	bool parseString(char* String, int len);

};


#endif
