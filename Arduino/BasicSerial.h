

#pragma once

typedef unsigned char  byte;

class BasicSerial {
public:
	virtual void begin(int baud) = 0;
	virtual int available() = 0;
	virtual int read() = 0;
	virtual int readBytes(char* buffer, int length) = 0;
	virtual void setTimeout(long time) = 0;
	virtual size_t write(byte* buffer, int length) = 0;
};


class Serial1T : public BasicSerial {
  public:
    virtual void begin(int baud);
    virtual int available();
    virtual int read();
    virtual int readBytes(char* buffer, int length);
    virtual void setTimeout(long time);
    virtual size_t write(byte* buffer, int length);
};
