// --- SENSOR LIBRARIES ---
#include "Adafruit_MAX31855.h"

// --- DISPLAY LIBRARIES ---
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- SERVO LIBRARY ---
#include <ESP32Servo.h>

// --- NEW HEATER CONTROL SETUP ---
#define HEATER_OFF_TEMP 150.0  // Temp (C) to turn the heater OFF
#define HEATER_ON_TEMP  130.0  // Temp (C) to turn the heater ON

#define SERVO_HEATER_OFF_POS 0   // Servo position for when the heater is OFF
#define SERVO_HEATER_ON_POS  180 // Servo position for when the heater is ON

// NEW: Define the delay before detaching the servo (in milliseconds)
#define SERVO_DETACH_DELAY 5000 

Servo myServo;
const int servoPin = 13;
bool isHeaterOn = false; // State variable to track if the heater is on or off

// NEW: Variable to store the time of the last servo movement
unsigned long lastServoMoveTime = 0;

// --- SENSOR SETUP ---
const int thermoDO  = 19;
const int thermoCS  = 5;
const int thermoCLK = 18;
Adafruit_MAX31855 thermocouple(thermoCLK, thermoCS, thermoDO);

// --- DISPLAY SETUP ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float maxTempC = -1000.0;

void setup() {
  Serial.begin(9600);
  
  // Correctly attach the servo with the full pulse width range for an SG90
  myServo.attach(servoPin, 500, 2500);  
  
  myServo.write(SERVO_HEATER_OFF_POS); // Start with the heater OFF
  isHeaterOn = false;

  // NEW: Record the time of this initial movement
  lastServoMoveTime = millis();
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Water Heater");
  display.display();
  delay(1500);
}

void loop() {
  double currentTempC = thermocouple.readCelsius();

  // --- NEW THERMOSTAT LOGIC ---
  // If temp drops below the ON threshold AND the heater is currently off...
  if (currentTempC < HEATER_ON_TEMP && !isHeaterOn) {
    // NEW: Re-attach the servo if it was detached
    if (!myServo.attached()) {
      myServo.attach(servoPin, 500, 2500);
      delay(10); // Short delay to allow the servo to initialize
    }
    myServo.write(SERVO_HEATER_ON_POS); // Turn heater ON
    isHeaterOn = true;                  // Update state
    lastServoMoveTime = millis();       // NEW: Record the move time
    Serial.println("Temp is LOW. Turning heater ON.");
  }
  
  // If temp rises above the OFF threshold AND the heater is currently on...
  if (currentTempC > HEATER_OFF_TEMP && isHeaterOn) {
    // NEW: Re-attach the servo if it was detached
    if (!myServo.attached()) {
      myServo.attach(servoPin, 500, 2500);
      delay(10); // Short delay to allow the servo to initialize
    }
    myServo.write(SERVO_HEATER_OFF_POS); // Turn heater OFF
    isHeaterOn = false;                  // Update state
    lastServoMoveTime = millis();        // NEW: Record the move time
    Serial.println("Temp is HIGH. Turning heater OFF.");
  }

  // --- NEW: SERVO DETACH LOGIC ---
  // If the servo is currently attached AND 5 seconds have passed since the last move...
  if (myServo.attached() && (millis() - lastServoMoveTime > SERVO_DETACH_DELAY)) {
    myServo.detach(); // Detach the servo to save power and prevent overheating
    Serial.println("Servo detached to conserve power.");
  }
  
  // --- DISPLAY AND SENSOR LOGIC ---
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  uint8_t fault = thermocouple.readError();
  if (fault) {
    display.setTextSize(1);
    display.setCursor(0, 10);
    display.println("Sensor Fault!");
  } else {
    if (currentTempC > maxTempC) {
      maxTempC = currentTempC;
    }
    
    // --- Display Current Temperature ---
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Current:");
    display.setTextSize(2);
    display.setCursor(0, 10);
    display.print(currentTempC, 1);
    display.cp437(true);
    display.write(167);
    display.print("C");
    
    // --- Display Heater Status ---
    display.setTextSize(1);
    display.setCursor(0, 35);
    display.print("Heater:");
    display.setTextSize(2);
    display.setCursor(0, 45);
    display.print(isHeaterOn ? "ON" : "OFF"); // Use a ternary operator for clean code

  }

  display.display();
  delay(1000); // Check temperature every second
}
