/*
 * Mock serial port for testing Skirnir. Implement only .begin(), .available(), .read(), and .write(),
 * since those are what Skirnir uses
 */
#ifndef HARDWARESERIAL_H_INCLUDED
#define HARDWARESERIAL_H_INCLUDED

#include <cstddef>
#include <cstdint>

#define HARDWARESERIAL_INPUT_SIZE 300
#define HARDWARESERIAL_OUTPUT_SIZE 300

class HardwareSerial {
  public:
    // From Arduino HardwareSerial
    int available();
    int read();
    size_t write(uint8_t);
    size_t write(const char[]);
    
    // Additional API for testing
    HardwareSerial();
    HardwareSerial(uint8_t payload[], uint32_t size); // Same as calling constructor and then .addInput()
    uint8_t inputBuffer[HARDWARESERIAL_INPUT_SIZE];
    uint32_t inputAvailable;
    uint8_t outputBuffer[HARDWARESERIAL_OUTPUT_SIZE];
    uint32_t outputAvailable;
    void addInput(uint8_t payload[], uint32_t size);
};

#endif // ifndef
