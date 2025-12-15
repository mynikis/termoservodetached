#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Thermostat.h"
#include "MockHardware.h"

using namespace testing;

// Forward declarations for mock time functions in MockArduino.cpp
void set_mock_millis(unsigned long new_time);
void advance_mock_millis(unsigned long advance_ms);

class ThermostatTest : public Test {
protected:
    // Mocks for all hardware
    NiceMock<MockThermocouple> mockThermo;
    NiceMock<MockServo> mockServo;
    NiceMock<MockDisplay> mockDisplay;

    // The Thermostat instance under test
    std::unique_ptr<Thermostat> thermostat;

    void SetUp() override {
        // Reset mock time before each test
        set_mock_millis(0);

        // Create a new thermostat for each test, injecting the mocks
        thermostat = std::make_unique<Thermostat>(mockThermo, mockServo, mockDisplay);

        // Default expectations for display methods that are called in the loop
        // but are not the focus of most tests.
        ON_CALL(mockDisplay, clearDisplay).WillByDefault(Return());
        ON_CALL(mockDisplay, display).WillByDefault(Return());
    }
};

TEST_F(ThermostatTest, HeaterTurnsOnWhenTemperatureIsLow) {
    // Arrange: Set temperature below the ON threshold
    ON_CALL(mockThermo, readCelsius()).WillByDefault(Return(120.0));
    ON_CALL(mockThermo, readError()).WillByDefault(Return(0));

    // Expect the servo to be written to the ON position
    EXPECT_CALL(mockServo, write(180)); // SERVO_HEATER_ON_POS is 180

    // Act: Run the thermostat loop
    thermostat->loop();

    // Assert: Check if the heater state is now ON
    EXPECT_TRUE(thermostat->isHeaterOn());
}

TEST_F(ThermostatTest, HeaterTurnsOffWhenTemperatureIsHigh) {
    // Arrange: First, turn the heater on by setting a low temperature
    ON_CALL(mockThermo, readCelsius()).WillByDefault(Return(120.0));
    thermostat->loop();
    EXPECT_TRUE(thermostat->isHeaterOn());

    // Now, set temperature above the OFF threshold
    ON_CALL(mockThermo, readCelsius()).WillByDefault(Return(160.0));
    
    // Expect the servo to be written to the OFF position
    EXPECT_CALL(mockServo, write(0)); // SERVO_HEATER_OFF_POS is 0

    // Act: Run the thermostat loop again
    thermostat->loop();

    // Assert: Check if the heater state is now OFF
    EXPECT_FALSE(thermostat->isHeaterOn());
}

TEST_F(ThermostatTest, ServoDetachesAfterDelay) {
    // Arrange: Move the servo once
    ON_CALL(mockThermo, readCelsius()).WillByDefault(Return(120.0));
    ON_CALL(mockServo, attached()).WillByDefault(Return(true));
    thermostat->loop();
    
    // We expect detach NOT to be called immediately
    EXPECT_CALL(mockServo, detach()).Times(0);
    thermostat->loop();

    // Act: Advance time past the detach delay
    advance_mock_millis(6000); // SERVO_DETACH_DELAY is 5000

    // We now expect detach to be called
    EXPECT_CALL(mockServo, detach()).Times(1);
    thermostat->loop();
}

TEST_F(ThermostatTest, DisplayShowsCorrectTemperatureAndStatus_ON) {
    // Arrange
    ON_CALL(mockThermo, readCelsius()).WillByDefault(Return(99.9));
    ON_CALL(mockThermo, readError()).WillByDefault(Return(0));

    // Expectations for the display output
    {
        InSequence seq; // Verify calls are in the correct order
        EXPECT_CALL(mockDisplay, clearDisplay());
        EXPECT_CALL(mockDisplay, setTextColor(1));
        
        // Temperature
        EXPECT_CALL(mockDisplay, setTextSize(1));
        EXPECT_CALL(mockDisplay, setCursor(0, 0));
        EXPECT_CALL(mockDisplay, print(StrEq("Current:")));
        EXPECT_CALL(mockDisplay, setTextSize(2));
        EXPECT_CALL(mockDisplay, setCursor(0, 10));
        EXPECT_CALL(mockDisplay, print(99.9, 1));
        EXPECT_CALL(mockDisplay, cp437(true));
        EXPECT_CALL(mockDisplay, write(167));
        EXPECT_CALL(mockDisplay, print(StrEq("C")));

        // Heater Status (should be ON)
        EXPECT_CALL(mockDisplay, setTextSize(1));
        EXPECT_CALL(mockDisplay, setCursor(0, 35));
        EXPECT_CALL(mockDisplay, print(StrEq("Heater:")));
        EXPECT_CALL(mockDisplay, setTextSize(2));
        EXPECT_CALL(mockDisplay, setCursor(0, 45));
        EXPECT_CALL(mockDisplay, print(StrEq("ON")));

        EXPECT_CALL(mockDisplay, display());
    }

    // Act
    thermostat->loop();
}

TEST_F(ThermostatTest, DisplayShowsCorrectTemperatureAndStatus_OFF) {
    // Arrange: Set temperature to a normal level
    ON_CALL(mockThermo, readCelsius()).WillByDefault(Return(140.0));
    ON_CALL(mockThermo, readError()).WillByDefault(Return(0));

    // Expectations for the display output
    {
        InSequence seq;
        EXPECT_CALL(mockDisplay, clearDisplay());
        EXPECT_CALL(mockDisplay, setTextColor(1));
        
        // Temperature
        EXPECT_CALL(mockDisplay, setTextSize(1));
        EXPECT_CALL(mockDisplay, setCursor(0, 0));
        EXPECT_CALL(mockDisplay, print(StrEq("Current:")));
        EXPECT_CALL(mockDisplay, setTextSize(2));
        EXPECT_CALL(mockDisplay, setCursor(0, 10));
        EXPECT_CALL(mockDisplay, print(140.0, 1));
        EXPECT_CALL(mockDisplay, cp437(true));
        EXPECT_CALL(mockDisplay, write(167));
        EXPECT_CALL(mockDisplay, print(StrEq("C")));

        // Heater Status (should be OFF)
        EXPECT_CALL(mockDisplay, setTextSize(1));
        EXPECT_CALL(mockDisplay, setCursor(0, 35));
        EXPECT_CALL(mockDisplay, print(StrEq("Heater:")));
        EXPECT_CALL(mockDisplay, setTextSize(2));
        EXPECT_CALL(mockDisplay, setCursor(0, 45));
        EXPECT_CALL(mockDisplay, print(StrEq("OFF")));

        EXPECT_CALL(mockDisplay, display());
    }

    // Act
    thermostat->loop();
}