#ifndef MOCK_HARDWARE_H
#define MOCK_HARDWARE_H

#include "gmock/gmock.h"
#include "../src/Hardware.h"

class MockThermocouple : public IThermocouple {
public:
    MOCK_METHOD(double, readCelsius, (), (override));
    MOCK_METHOD(uint8_t, readError, (), (override));
};

class MockServo : public IServo {
public:
    MOCK_METHOD(void, write, (int value), (override));
    MOCK_METHOD(void, attach, (int pin, int min, int max), (override));
    MOCK_METHOD(void, detach, (), (override));
    MOCK_METHOD(bool, attached, (), (override));
};

class MockDisplay : public IDisplay {
public:
    MOCK_METHOD(void, clearDisplay, (), (override));
    MOCK_METHOD(void, display, (), (override));
    MOCK_METHOD(void, setTextSize, (int size), (override));
    MOCK_METHOD(void, setTextColor, (int color), (override));
    MOCK_METHOD(void, setCursor, (int x, int y), (override));
    MOCK_METHOD(void, print, (const char* str), (override));
    MOCK_METHOD(void, print, (double val, int precision), (override));
    MOCK_METHOD(void, println, (const char* str), (override));
    MOCK_METHOD(void, cp437, (bool on), (override));
    MOCK_METHOD(void, write, (uint8_t c), (override));
};

#endif // MOCK_HARDWARE_H
