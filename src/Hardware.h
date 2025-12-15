#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>

class IThermocouple {
public:
    virtual ~IThermocouple() {}
    virtual double readCelsius() = 0;
    virtual uint8_t readError() = 0;
};

class IServo {
public:
    virtual ~IServo() {}
    virtual void write(int value) = 0;
    virtual void attach(int pin, int min, int max) = 0;
    virtual void detach() = 0;
    virtual bool attached() = 0;
};

// A simplified display interface for this project
class IDisplay {
public:
    virtual ~IDisplay() {}
    virtual void clearDisplay() = 0;
    virtual void display() = 0;
    virtual void setTextSize(int size) = 0;
    virtual void setTextColor(int color) = 0;
    virtual void setCursor(int x, int y) = 0;
    virtual void print(const char* str) = 0;
    virtual void print(double val, int precision) = 0;
    virtual void println(const char* str) = 0;
    virtual void cp437(bool on) = 0;
    virtual void write(uint8_t c) = 0;
};

#endif // HARDWARE_H
