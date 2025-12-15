#include "Thermostat.h"
#include <string> // Required for std::to_string
#include <algorithm> // Required for std::sort

// Definition for the external millis() function.
// In a test environment, we will link a mock implementation.
extern "C" unsigned long millis();

Thermostat::Thermostat(IThermocouple& thermo, IServo& servo, IDisplay& display)
    : thermocouple(thermo),
      myServo(servo),
      display(display),
      heaterOn(false),
      lastServoMoveTime(0),
      maxTempC(-1000.0) {}

void Thermostat::setup() {
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
    // 1. Read raw temperature and add to our buffer
    double rawTemp = thermocouple.readCelsius();
    tempReadings.push_back(rawTemp);

    // 2. Trim the buffer if it's too large
    if (tempReadings.size() > MEDIAN_WINDOW_SIZE) {
        tempReadings.erase(tempReadings.begin());
    }

    // 3. Only proceed if we have a full window of readings
    if (tempReadings.size() == MEDIAN_WINDOW_SIZE) {
        double medianTemp = getMedianTemperature();

        // 4. Use the stable median temperature for all logic
        updateHeaterState(medianTemp);
        manageServoAttachment();
        updateDisplay(medianTemp);
    } else {
        // Optional: Display a "gathering data" message until the buffer is full
        display.clearDisplay();
        display.setCursor(0, 10);
        display.setTextSize(1);
        display.print("Gathering data... ");
        display.print(std::to_string(tempReadings.size()).c_str());
        display.print("/");
        display.print(std::to_string(MEDIAN_WINDOW_SIZE).c_str());
        display.display();
    }
}

double Thermostat::getMedianTemperature() {
    // Create a copy of the vector to avoid modifying the original
    std::vector<double> sortedReadings = tempReadings;
    
    // Sort the copy
    std::sort(sortedReadings.begin(), sortedReadings.end());
    
    // Return the middle element
    return sortedReadings[sortedReadings.size() / 2];
}

void Thermostat::updateHeaterState(double currentTempC) {
    if (currentTempC < HEATER_ON_TEMP && !heaterOn) {
        if (!myServo.attached()) {
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
        display.print("Median Temp:"); // Changed label
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
