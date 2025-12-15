// This is the main firmware file for the ESP32.
// It initializes the hardware, wraps it in adapter classes,
// and runs the testable Thermostat logic.

// --- Real Hardware Libraries ---
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MAX31855.h>
#include <ESP32Servo.h>

// --- Project Files ---
#include "../src/Thermostat.h"
#include "ArduinoAdapters.h"

// --- Hardware Pin Definitions ---
const int SERVO_PIN = 13;
const int THERMO_DO  = 19;
const int THERMO_CS  = 5;
const int THERMO_CLK = 18;

// --- Real Hardware Objects ---
Adafruit_MAX31855 realThermocouple(THERMO_CLK, THERMO_CS, THERMO_DO);
Adafruit_SSD1306 realDisplay(128, 64, &Wire, -1);
Servo realServo;

// --- Adapter Objects ---
ThermocoupleAdapter thermocoupleAdapter(realThermocouple);
DisplayAdapter displayAdapter(realDisplay);
ServoAdapter servoAdapter(realServo);

// --- The Main Thermostat Application ---
Thermostat thermostat(thermocoupleAdapter, servoAdapter, displayAdapter);

void setup() {
  Serial.begin(9600);

  // --- Initialize Real Hardware ---
  if (!realDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  
  // Attach the servo with its specific pin and pulse width range
  realServo.attach(SERVO_PIN, 500, 2500);

  // Run the application's setup routine
  thermostat.setup();

  // A small delay to allow things to settle before the first loop
  delay(1500);
}

void loop() {
  // Run the main application loop
  thermostat.loop();

  // The original sketch had a 1-second delay here.
  // This is important to prevent busy-looping and to control
  // the sampling frequency of the thermostat.
  delay(1000);
}