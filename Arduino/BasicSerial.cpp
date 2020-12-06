#include <Arduino.h>
#include "BasicSerial.h"

void Serial1T::begin(int baud) { return Serial1.begin(baud); }
int Serial1T::available() {return Serial1.available(); }
int Serial1T::read() { return Serial1.read(); }
int Serial1T::readBytes(char* buffer, int length) {return Serial1.readBytes(buffer, length); }
void Serial1T::setTimeout(long time) { return Serial1.setTimeout(time);} 
size_t Serial1T::write(byte* buffer, int length) { return Serial1.write(buffer, length);} 
