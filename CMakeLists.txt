# CMake minimum version
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(ThermostatProject CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Google Test ---
# Add Google Test framework for unit testing
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows:
# FetchContent_MakeAvailable(googletest)
# For Linux/macOS:
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# --- Project Source Files ---
# The source files are added directly to the test executable

# --- Testing ---
# Enable testing for the project
enable_testing()

# Add the test executable
add_executable(
  thermostat_tests
  src/Thermostat.cpp
  test/main.cpp
  test/ThermostatTests.cpp
  mocks/MockArduino.cpp
)

target_include_directories(thermostat_tests PRIVATE src mocks)

# Link Google Mock to the test executable
target_link_libraries(
  thermostat_tests
  gmock_main
)

# Include Google Test in the test project
include(GoogleTest)
# Discover and add tests to the CTest runner
gtest_discover_tests(thermostat_tests)
