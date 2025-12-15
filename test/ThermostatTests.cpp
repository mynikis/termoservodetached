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
    NiceMock<MockThermocouple> mockThermo;
    NiceMock<MockServo> mockServo;
    NiceMock<MockDisplay> mockDisplay;
    std::unique_ptr<Thermostat> thermostat;

    void SetUp() override {
        set_mock_millis(0);
        thermostat = std::make_unique<Thermostat>(mockThermo, mockServo, mockDisplay);
        ON_CALL(mockDisplay, clearDisplay).WillByDefault(Return());
        ON_CALL(mockDisplay, display).WillByDefault(Return());
        ON_CALL(mockThermo, readError()).WillByDefault(Return(0));
    }

    // Helper to run the loop N times to fill the median buffer
    void runLoop(int times) {
        for (int i = 0; i < times; ++i) {
            thermostat->loop();
        }
    }
};

TEST_F(ThermostatTest, DoesNotActUntilBufferIsFull) {
    ON_CALL(mockThermo, readCelsius()).WillByDefault(Return(120.0));
    
    EXPECT_CALL(mockServo, write(_)).Times(0);

    // Run the loop 6 times, one less than the window size (7)
    runLoop(6);

    ASSERT_FALSE(thermostat->isHeaterOn());
}

TEST_F(ThermostatTest, HeaterTurnsOnWhenMedianIsLow) {
    ON_CALL(mockThermo, readCelsius()).WillByDefault(Return(120.0));
    
    EXPECT_CALL(mockServo, write(180)).Times(1);

    runLoop(7);

    ASSERT_TRUE(thermostat->isHeaterOn());
}

TEST_F(ThermostatTest, HeaterTurnsOffWhenMedianIsHigh) {
    ON_CALL(mockThermo, readCelsius()).WillByDefault(Return(120.0));
    runLoop(7);
    ASSERT_TRUE(thermostat->isHeaterOn());

    ON_CALL(mockThermo, readCelsius()).WillByDefault(Return(160.0));
    EXPECT_CALL(mockServo, write(0)).Times(1);
    
    runLoop(4); 

    ASSERT_FALSE(thermostat->isHeaterOn());
}

TEST_F(ThermostatTest, MedianFilterRejectsOutliers) {
    EXPECT_CALL(mockThermo, readCelsius())
        .WillOnce(Return(125.0))
        .WillOnce(Return(500.0)) // Outlier
        .WillOnce(Return(126.0))
        .WillOnce(Return(-10.0)) // Outlier
        .WillOnce(Return(124.0))
        .WillOnce(Return(125.5))
        .WillOnce(Return(124.5));

    EXPECT_CALL(mockServo, write(180)).Times(1);

    runLoop(7);

    ASSERT_TRUE(thermostat->isHeaterOn());
}

TEST_F(ThermostatTest, DisplayShowsGatheringDataWhileBuffering) {
    ON_CALL(mockThermo, readCelsius()).WillByDefault(Return(100.0));

    // For loop 1 (size 1)
    {
        InSequence seq;
        EXPECT_CALL(mockDisplay, clearDisplay());
        EXPECT_CALL(mockDisplay, setCursor(0, 10));
        EXPECT_CALL(mockDisplay, setTextSize(1));
        EXPECT_CALL(mockDisplay, print(StrEq("Gathering data... ")));
        EXPECT_CALL(mockDisplay, print(StrEq("1")));
        EXPECT_CALL(mockDisplay, print(StrEq("/")));
        EXPECT_CALL(mockDisplay, print(StrEq("7")));
        EXPECT_CALL(mockDisplay, display());
    }
    thermostat->loop();

    // For loop 2 (size 2)
    {
        InSequence seq;
        EXPECT_CALL(mockDisplay, clearDisplay());
        EXPECT_CALL(mockDisplay, setCursor(0, 10));
        EXPECT_CALL(mockDisplay, setTextSize(1));
        EXPECT_CALL(mockDisplay, print(StrEq("Gathering data... ")));
        EXPECT_CALL(mockDisplay, print(StrEq("2")));
        EXPECT_CALL(mockDisplay, print(StrEq("/")));
        EXPECT_CALL(mockDisplay, print(StrEq("7")));
        EXPECT_CALL(mockDisplay, display());
    }
    thermostat->loop();
    
    // For loop 3 (size 3)
    {
        InSequence seq;
        EXPECT_CALL(mockDisplay, clearDisplay());
        EXPECT_CALL(mockDisplay, setCursor(0, 10));
        EXPECT_CALL(mockDisplay, setTextSize(1));
        EXPECT_CALL(mockDisplay, print(StrEq("Gathering data... ")));
        EXPECT_CALL(mockDisplay, print(StrEq("3")));
        EXPECT_CALL(mockDisplay, print(StrEq("/")));
        EXPECT_CALL(mockDisplay, print(StrEq("7")));
        EXPECT_CALL(mockDisplay, display());
    }
    thermostat->loop();
}

TEST_F(ThermostatTest, DisplayShowsCorrectMedianTemperatureWhenBufferIsFull) {
    EXPECT_CALL(mockThermo, readCelsius())
        .WillOnce(Return(140.0))
        .WillOnce(Return(141.0))
        .WillOnce(Return(142.0))
        .WillOnce(Return(139.0))
        .WillOnce(Return(138.0))
        .WillOnce(Return(137.0))
        .WillOnce(Return(136.0));

    // Allow calls made during the buffering phase.
    // NiceMock usually handles unexpected calls, but explicit AnyNumber() helps clarity.
    EXPECT_CALL(mockDisplay, clearDisplay()).Times(AnyNumber());
    EXPECT_CALL(mockDisplay, setTextColor(_)).Times(AnyNumber());
    EXPECT_CALL(mockDisplay, setTextSize(_)).Times(AnyNumber());
    EXPECT_CALL(mockDisplay, setCursor(_, _)).Times(AnyNumber());
    EXPECT_CALL(mockDisplay, print(_)).Times(AnyNumber()); // General print, if not specific
    EXPECT_CALL(mockDisplay, display()).Times(AnyNumber());

    // Now, set specific expectations for the *final* display state (after buffer is full)
    // These expectations will take precedence or match the calls on the 7th loop.
    EXPECT_CALL(mockDisplay, clearDisplay()).Times(AtLeast(1)); // It gets called one last time
    EXPECT_CALL(mockDisplay, setTextColor(1)).Times(AtLeast(1));
    
    EXPECT_CALL(mockDisplay, print(StrEq("Median Temp:"))).Times(1);
    EXPECT_CALL(mockDisplay, print(DoubleEq(139.0), 1)).Times(1);
    EXPECT_CALL(mockDisplay, print(StrEq("C"))).Times(1);
    EXPECT_CALL(mockDisplay, print(StrEq("Heater:"))).Times(1);
    EXPECT_CALL(mockDisplay, print(StrEq("OFF"))).Times(1);
    EXPECT_CALL(mockDisplay, display()).Times(AtLeast(1));

    runLoop(7);
}
