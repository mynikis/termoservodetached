#include "Thermostat.h"

// Definition for the external millis() function.
// In a test environment, we will link a mock implementation.
unsigned long millis();

Thermostat::Thermostat(IThermocouple& thermo, IServo& servo, IDisplay& display)
    : thermocouple(thermo),
      myServo(servo),
      display(display),
      heaterOn(false),
      lastServoMoveTime(0),
      maxTempC(-1000.0) {}

void Thermostat::setup() {
    // The servo pin and range are now managed by the adapter/main.cpp
    myServo.write(SERVO_HEATER_OFF_POS);
    heaterOn = false;
    lastServoMoveTime = millis();

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(1); // Assuming 1 is a valid color like WHITE
    display.setCursor(0, 0);
    display.println("Water Heater");
    display.display();
}

void Thermostat::loop() {
    double currentTempC = thermocouple.readCelsius();
    updateHeaterState(currentTempC);
    manageServoAttachment();
    updateDisplay(currentTempC);
}

void Thermostat::updateHeaterState(double currentTempC) {
    if (currentTempC < HEATER_ON_TEMP && !heaterOn) {
        if (!myServo.attached()) {
            // Pin and range are managed by the concrete implementation
            myServo.attach(0, 0, 0); 
        }
        myServo.write(SERVO_HEATER_ON_POS);
        heaterOn = true;
        lastServoMoveTime = millis();
    } else if (currentTempC > HEATER_OFF_TEMP && heaterOn) {
        if (!myServo.attached()) {
            myServo.attach(0, 0, 0);
        }
        myServo.write(SERVO_HEATER_OFF_POS);
        heaterOn = false;
        lastServoMoveTime = millis();
    }
}

void Thermostat::manageServoAttachment() {
    if (myServo.attached() && (millis() - lastServoMoveTime > SERVO_DETACH_DELAY)) {
        myServo.detach();
    }
}

void Thermostat::updateDisplay(double currentTempC) {
    display.clearDisplay();
    display.setTextColor(1);

    if (thermocouple.readError()) {
        display.setTextSize(1);
        display.setCursor(0, 10);
        display.println("Sensor Fault!");
    } else {
        if (currentTempC > maxTempC) {
            maxTempC = currentTempC;
        }

        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("Current:");
        display.setTextSize(2);
        display.setCursor(0, 10);
        display.print(currentTempC, 1);
        display.cp437(true);
        display.write(167); // Degree symbol
        display.print("C");

        display.setTextSize(1);
        display.setCursor(0, 35);
        display.print("Heater:");
        display.setTextSize(2);
        display.setCursor(0, 45);
        display.print(heaterOn ? "ON" : "OFF");
    }

    display.display();
}

bool Thermostat::isHeaterOn() const {
    return heaterOn;
}

unsigned long Thermostat::getLastServoMoveTime() const {
    return lastServoMoveTime;
}
