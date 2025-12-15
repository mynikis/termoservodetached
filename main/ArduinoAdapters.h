#ifndef ARDUINO_ADAPTERS_H
#define ARDUINO_ADAPTERS_H

#include "../src/Hardware.h"
#include <Adafruit_MAX31855.h>
#include <ESP32Servo.h>
#include <Adafruit_SSD1306.h>

// --- Thermocouple Adapter ---
class ThermocoupleAdapter : public IThermocouple {
public:
    ThermocoupleAdapter(Adafruit_MAX31855& thermo) : realThermo(thermo) {}
    double readCelsius() override { return realThermo.readCelsius(); }
    uint8_t readError() override { return realThermo.readError(); }
private:
    Adafruit_MAX31855& realThermo;
};

// --- Servo Adapter ---
class ServoAdapter : public IServo {
public:
    ServoAdapter(Servo& servo) : realServo(servo) {}

    void write(int value) override { realServo.write(value); }
    void attach(int pin, int min, int max) override { 
        // The real attach is called once in setup, so this can be a no-op
        // or re-attach if needed. The pin/min/max are stored on creation.
        realServo.attach(pin, min, max);
    }
    void detach() override { realServo.detach(); }
    bool attached() override { return realServo.attached(); }

private:
    Servo& realServo;
};

// --- Display Adapter ---
class DisplayAdapter : public IDisplay {
public:
    DisplayAdapter(Adafruit_SSD1306& display) : realDisplay(display) {}

    void clearDisplay() override { realDisplay.clearDisplay(); }
    void display() override { realDisplay.display(); }
    void setTextSize(int size) override { realDisplay.setTextSize(size); }
    void setTextColor(int color) override { realDisplay.setTextColor(color); }
    void setCursor(int x, int y) override { realDisplay.setCursor(x, y); }
    void print(const char* str) override { realDisplay.print(str); }
    void print(double val, int precision) override { realDisplay.print(val, precision); }
    void println(const char* str) override { realDisplay.println(str); }
    void cp437(bool on) override { realDisplay.cp437(on); }
    void write(uint8_t c) override { realDisplay.write(c); }

private:
    Adafruit_SSD1306& realDisplay;
};

#endif // ARDUINO_ADAPTERS_H
