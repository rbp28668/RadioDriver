#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#include <Windows.h>
#include <fstream>

typedef unsigned char  byte;

class SerialT {

	bool full;
	std::string line;
	std::ifstream ifs;

public:
	SerialT() : full(false) {}

	void begin(int baudRate);
	int available();
	int readBytes(char* buffer, int bytesToRead);

	void print(char ch);
	void print(const char* msg);
	void println(const char* msg);
};

class IntervalTimer {
	void (*onTick)();
	int uSDelay;
	uintptr_t threadHandle;
	unsigned thrdaddr;
	static unsigned __stdcall threadFunction(void* obj);

public:
	void begin(void (*fn)(), int uS);
};

enum {
	A0,
	A1,
	A2,
	A3,
	A4,
	A5,
	A6,
	A7,
	A8,
	A9,
	A10,
	A11,
	A12,
	A13,
	A14
};

enum class LevelT {
	INPUT,
	OUTPUT
};

enum {
	LOW,
	HIGH
};

extern void noInterrupts();
extern void interrupts();

extern void analogWriteResolution(int nBits);
extern void analogWrite(int pin, int value);

extern void pinMode(int pin, int mode);
extern void digitalWrite(int pin, int state);
extern int digitalRead(int pin);

extern void delayMicroseconds(int uS);
extern void delay(int mS);

//extern void memcpy(void* dest, const void* src, size_t nBytes);

extern SerialT Serial;


