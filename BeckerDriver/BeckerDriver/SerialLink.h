#pragma once
#include <string>
#include <Windows.h>
#include "BasicSerial.h"

class SerialLink : public BasicSerial
{
	boolean connected;   // true if connected
	HANDLE hComm;		// handle to port
	DWORD dwToWrite;	// bytes to write
	DWORD bytesRead;
	char readBuffer[256];

	void readBytes();
	void remove(int count);

public:
	enum class ErrorT {
		SUCCESS = 0,
		CANNOT_CONNECT,
		BUILD_DCB,
		COMMS_SET_STATE,
		COMMS_SET_TIMEOUTS
	};

	SerialLink();
	SerialLink(const char* pszPort);
	virtual ~SerialLink(void);

	ErrorT connect(const char* pszPort);
	void disconnect();
	boolean isConnected();

	// Arduino functions
	virtual void begin(int baud);
	virtual int available();
	virtual int read();
	virtual int readBytes(char* buffer, int length);
	virtual void setTimeout(long time);
	virtual size_t write(byte value);
	virtual size_t write(const char* string);
	virtual size_t write(byte* buffer, int length);

};

