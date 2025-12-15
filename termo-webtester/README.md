# Thermostat Visual Tester

This is a simple, standalone web application for visually testing the logic of the C++ thermostat.

## How it Works

The thermostat logic has been re-implemented in client-side JavaScript (`public/app.js`) for simulation purposes. This allows for rapid, visual testing of the temperature thresholds without needing to compile or upload code to an ESP32.

This web app runs entirely locally and does not require any external packages to be installed.

## How to Run

1.  Make sure you have Python 3 installed.
2.  Navigate to this directory (`termo-webtester`).
3.  Start the simple web server using the command:
    ```sh
    python3 server.py
    ```
4.  Open your web browser and navigate to `http://localhost:8000`.
