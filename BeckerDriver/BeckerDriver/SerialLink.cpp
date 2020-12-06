#include <assert.h>
#include <iostream>
#include "SerialLink.h"


SerialLink::SerialLink()
	: dwToWrite(0),
	bytesRead(0),
	hComm(INVALID_HANDLE_VALUE),
	connected(false)
{
}

SerialLink::SerialLink(const char* pszPort)
	: dwToWrite(0),
	bytesRead(0),
	hComm(INVALID_HANDLE_VALUE),
	connected(false)
{
	assert(pszPort);
	connect(pszPort);

}


SerialLink::~SerialLink(void)
{
	disconnect();
}

SerialLink::ErrorT SerialLink::connect(const char* pszPort)
{
	assert(this);
	assert(pszPort);

	hComm = ::CreateFileA(pszPort,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		0, // Not overlapped
		0);
	if (hComm == INVALID_HANDLE_VALUE) {
		return ErrorT::CANNOT_CONNECT;
	}


	DCB dcb;
	::memset(&dcb, 0, sizeof(dcb));

	dcb.DCBlength = sizeof(dcb);
	if (!BuildCommDCBA("9600,n,8,1", &dcb)) {
		return ErrorT::BUILD_DCB;
	}
	dcb.fOutxCtsFlow = 0;
	dcb.fOutxDsrFlow = 0;
	dcb.fDtrControl = 0;
	dcb.fDsrSensitivity = 0;
	dcb.fOutX = 0;
	dcb.fInX = 0;
	dcb.fRtsControl = 0;
	dcb.fAbortOnError = 0;


	if (!SetCommState(hComm, &dcb)) {
		return ErrorT::COMMS_SET_STATE;
	}

	// Set timeouts - see https://docs.microsoft.com/en-gb/windows/desktop/api/winbase/nf-winbase-getcommtimeouts
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 100;       // 100mS break terminates message
	timeouts.ReadTotalTimeoutConstant = 100;   // allow total of 100mS
	timeouts.ReadTotalTimeoutMultiplier = 5;  // + 5mS per character
	timeouts.WriteTotalTimeoutMultiplier = 10;  // 10mS per char
	timeouts.WriteTotalTimeoutConstant = 100;   // + 100mS per message

	if (!::SetCommTimeouts(hComm, &timeouts)) {
		return ErrorT::COMMS_SET_TIMEOUTS;
	}

	connected = true;
	return ErrorT::SUCCESS;
}

void SerialLink::disconnect()
{
	assert(this);
	if (hComm != INVALID_HANDLE_VALUE) {
		::CloseHandle(hComm);
	}

	hComm = INVALID_HANDLE_VALUE;
	connected = false;
}

boolean SerialLink::isConnected()
{
	assert(this);
	return connected;
}




void SerialLink::readBytes() {
	assert(this);
	assert(connected);

	// Just read one at a time into the buffer.
	DWORD count = 0;
	do {
		if (bytesRead >= sizeof(readBuffer)) {
			break;
		}
		ReadFile(hComm, readBuffer + bytesRead, 1, &count, NULL);
		bytesRead += count;
	} while (count > 0);
}

void SerialLink::remove(int count)
{
	int toMove = bytesRead - count;
	char* dest = readBuffer;
	char* src = readBuffer + count;
	while (toMove--) {
		*dest++ = *src++;
	}
	bytesRead -= count;
}



void SerialLink::begin(int baud)
{
}

int SerialLink::available()
{
	readBytes();
	return bytesRead;
}

int SerialLink::read()
{
	int ch = -1;
	readBytes();
	if (bytesRead > 0) {
		ch = readBuffer[0];
		remove(1);
	}
	return ch;
}

int SerialLink::readBytes(char* buffer, int length)
{
	readBytes();

	int toRead = length;
	if (bytesRead < toRead) {
		toRead = bytesRead;
	}
	
	memcpy(buffer, readBuffer, toRead);
	remove(toRead);

	return toRead;
}

void SerialLink::setTimeout(long time)
{
}

size_t SerialLink::write(byte value)
{
	assert(this);
	assert(connected);

	DWORD dwWritten;
	WriteFile(hComm, &value, 1, &dwWritten, NULL);
	return size_t(dwWritten);
}

size_t SerialLink::write(const char* string)
{
	assert(this);
	assert(connected);

	DWORD dwWritten;
	dwToWrite = (DWORD)strlen(string);
	WriteFile(hComm, string, dwToWrite, &dwWritten, NULL);
	return size_t(dwWritten);
}

size_t SerialLink::write(byte* buffer, int length)
{
	assert(this);
	assert(connected);

	DWORD dwWritten;
	dwToWrite = (DWORD)length;
	WriteFile(hComm, buffer, dwToWrite, &dwWritten, NULL);
	return size_t(dwWritten);
}
