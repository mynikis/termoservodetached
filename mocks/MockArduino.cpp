#include <chrono>

// This is the mock implementation of the millis() function.
// It provides a controllable way to simulate the passage of time in tests.

namespace {
    // We can use a static variable to hold the mock time.
    // A more advanced implementation could use a thread-local variable
    // or a dedicated time-provider class.
    unsigned long mock_millis_time = 0;
}

// The actual mock function that will be linked during tests.
extern "C" unsigned long millis() {
    return mock_millis_time;
}

// A helper function for tests to control the mock time.
void set_mock_millis(unsigned long new_time) {
    mock_millis_time = new_time;
}

void advance_mock_millis(unsigned long advance_ms) {
    mock_millis_time += advance_ms;
}
