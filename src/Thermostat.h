#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include "Hardware.h"

// This function will be provided by the Arduino framework in production
// and by a mock implementation in the test environment.
extern "C" unsigned long millis();

class Thermostat {
public:
    Thermostat(IThermocouple& thermo, IServo& servo, IDisplay& display);

    void setup();
    void loop();

    // These methods are public for easier testing
    bool isHeaterOn() const;
    unsigned long getLastServoMoveTime() const;

private:
    // Constants
    static constexpr double HEATER_OFF_TEMP = 150.0;
    static constexpr double HEATER_ON_TEMP  = 130.0;
    static constexpr int SERVO_HEATER_OFF_POS = 0;
    static constexpr int SERVO_HEATER_ON_POS  = 180;
    static constexpr unsigned long SERVO_DETACH_DELAY = 5000;

    // Hardware interfaces
    IThermocouple& thermocouple;
    IServo& myServo;
    IDisplay& display;

    // State
    bool heaterOn;
    unsigned long lastServoMoveTime;
    float maxTempC;

    void updateHeaterState(double currentTempC);
    void updateDisplay(double currentTempC);
    void manageServoAttachment();
};

#endif // THERMOSTAT_H
