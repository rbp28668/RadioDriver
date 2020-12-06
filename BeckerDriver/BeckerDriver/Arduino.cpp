#include "Arduino.h"
#include <synchapi.h>
#include <process.h>    /* _beginthread, _endthread */
#include <iostream>
#include <fstream>
#include <string>


CRITICAL_SECTION interruptGuard;
SerialT Serial;

const char* msg = "L0000010000000100000001000000010000000100";
const char* ping = "P\r\n";

// class SerialT

void SerialT::begin(int baudRate) {
	ifs.open("E:/TEMP/cutter.out");
}

int SerialT::available() {
	if (full) return (int)strlen(ping);

	if (std::getline(ifs, line)) {
		return (int)line.size();
	}
	/*else {
		std::cout << "Finished" << std::endl;
	}*/

	return 0;
}

int SerialT::readBytes(char* buffer, int bytesToRead) {
	if (full) {
		strncpy(buffer, ping, sizeof(ping));
	}
	else {
		strncpy(buffer, line.c_str(), bytesToRead);
	}
	return bytesToRead;
}

void SerialT::print(char ch) {
	std::cout << ch;
}

void SerialT::print(const char* msg) {
	std::cout << msg;
}

void SerialT::println(const char* msg) {
	//std::cout << msg << std::endl;
	full = msg[1] == 'F';
}



// class IntervalTimer {
void IntervalTimer::begin(void (*fn)(), int uS) {
	onTick = fn;
	uSDelay = uS;

	::_beginthreadex(0, 0, &threadFunction, this, 0, &thrdaddr);
}

unsigned __stdcall IntervalTimer::threadFunction(void* obj) {
	IntervalTimer* pTimer = static_cast<IntervalTimer*>(obj);

	__int64 time1 = 0, time2 = 0, freq = 0;
	::QueryPerformanceFrequency((LARGE_INTEGER*)&freq); // counts per sec
	long long ticks = (1000000 * pTimer->uSDelay) / freq;

	for (;;) {

		::EnterCriticalSection(&interruptGuard);
		pTimer->onTick();
		::LeaveCriticalSection(&interruptGuard);

		::QueryPerformanceCounter((LARGE_INTEGER*)&time1);
		do {
			::QueryPerformanceCounter((LARGE_INTEGER*)&time2);
		} while ((time2 - time1) < ticks);

	}
	_endthreadex(0);
	return 0;
}



void noInterrupts() {
	::EnterCriticalSection(&interruptGuard);
}
void interrupts() {
	::LeaveCriticalSection(&interruptGuard);
}

void analogWriteResolution(int nBits) {

}
void analogWrite(int pin, int value) {

}

void pinMode(int pin, int mode) {

}
void digitalWrite(int pin, int state) {

}
int digitalRead(int pin) {
	return LOW;
}

void delayMicroseconds(int uS) {

}

void delay(int mS) {
	::Sleep(mS);
}

//void memcpy(void* dest, const void* src, size_t nBytes) {
//	while (nBytes-- > 0) {
//		*(char*)dest = *(const char*)src;
//	}
//}

void initArduinoEmulation(){
	::InitializeCriticalSection(&interruptGuard);
}

