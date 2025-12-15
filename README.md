# ESP32 Thermostat with Median Filter

This project implements a smart thermostat for an ESP32 microcontroller, featuring a thermocouple sensor, an OLED display, and a servo-controlled heater. The core logic is designed with testability in mind, separating hardware concerns from the main application logic.

## Features

-   **Temperature Monitoring:** Reads temperature using a MAX31855 thermocouple.
-   **Heater Control:** Controls a servo motor to toggle a heater based on temperature thresholds.
-   **OLED Display:** Shows current temperature, heater status, and buffering information.
-   **7-Point Median Filter:** Implemented for robust temperature readings, smoothing out fluctuations and rejecting transient erroneous values. This helps prevent undesirable actions triggered by noisy sensor data.
-   **Unit Testing:** Core thermostat logic is unit-tested using Google Test and Google Mock for local development and verification.
-   **Web-based Visualizer:** A simple web application for simulating and visually testing the thermostat logic without requiring physical hardware.

## Project Structure

-   `src/`: Contains the core `Thermostat` C++ class and hardware interfaces.
-   `main/`: Contains the Arduino (`.ino`) firmware for the ESP32, which integrates the `Thermostat` class with actual hardware drivers.
-   `mocks/`: Contains mock implementations of hardware interfaces for unit testing.
-   `test/`: Contains the C++ unit tests for the `Thermostat` logic.
-   `termo-webtester/`: A self-contained directory for the web-based visualizer.

## Building and Running C++ Unit Tests

To ensure the core thermostat logic is functioning correctly, you can build and run the unit tests on your local machine:

1.  Navigate to the project root directory:
    ```bash
    cd /home/alex/Arduino/termoservodetached
    ```
2.  Create a build directory and navigate into it:
    ```bash
    mkdir build
    cd build
    ```
3.  Configure the project with CMake:
    ```bash
    cmake ..
    ```
4.  Compile the tests:
    ```bash
    make -j$(nproc) # Use -j$(nproc) for parallel compilation
    ```
5.  Run the tests:
    ```bash
    ./thermostat_tests
    ```
    All tests should pass, confirming the median filter and control logic.

## Using the Web-based Visualizer

The `termo-webtester` provides a convenient way to interact with and test the thermostat logic visually in your browser, without needing an ESP32.

1.  Navigate to the `termo-webtester` directory:
    ```bash
    cd /home/alex/Arduino/termoservodetached/termo-webtester
    ```
2.  Start the simple Python web server (ensure you have Python 3 installed):
    ```bash
    python3 server.py
    ```
    (If you get an `Address already in use` error, another process is using port 8000. You might need to kill it or change the `PORT` variable in `server.py` to `8001`.)
3.  Open your web browser and navigate to `http://localhost:8000`.

You can use the slider to simulate temperature changes and observe the median temperature and heater status update in real-time.

## Flashing to ESP32

To deploy this project to an ESP32, you would typically open the `main/app.ino` file in the Arduino IDE (or PlatformIO), ensure all necessary libraries are installed, and then upload the sketch. The `main/app.ino` file is configured to use the `Thermostat` class and its median filter logic.
