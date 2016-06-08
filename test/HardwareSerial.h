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
    uint32_t inputAvailable = 0;
    uint32_t outputAvailable = 0;
    uint8_t inputBuffer[HARDWARESERIAL_INPUT_SIZE];
    uint8_t outputBuffer[HARDWARESERIAL_OUTPUT_SIZE];
    
    HardwareSerial() {}
    
    // Same as calling constructor and then .addInput()
    HardwareSerial(uint8_t payload[], uint32_t size) {
      addInput(payload, size);
    }
    
    void addInput(uint8_t payload[], uint32_t size) {
      uint32_t size_accepted = size < HARDWARESERIAL_INPUT_SIZE - inputAvailable ? size : HARDWARESERIAL_INPUT_SIZE - inputAvailable;
      
      for(uint32_t i = 0; i < size_accepted; ++i) {
        inputBuffer[inputAvailable + i] = payload[i];
      }
      
      inputAvailable += size_accepted;
    }
    
    // Methods below are from Arduino HardwareSerial:
    
    int available() {
      return (int) inputAvailable;
    }
    
    int read() {
      if(inputAvailable == 0) {
        return -1;
      }
      
      int result = inputBuffer[0];
      
      --inputAvailable;
      
      for(uint32_t i = 0; i < inputAvailable; ++i) {
        inputBuffer[i] = inputBuffer[i + 1];
      }
      
      return result;
    }
    
    size_t write(uint8_t c) {
      if(outputAvailable < HARDWARESERIAL_OUTPUT_SIZE) {
        outputBuffer[outputAvailable] = c;
        ++outputAvailable;
        
        return 1;
      }
      
      return 0;
    }
    
    size_t write(const char s[]) {
      size_t written = 0;
      
      while(outputAvailable < HARDWARESERIAL_OUTPUT_SIZE && s[written] != '\0') {
        outputBuffer[outputAvailable] = (uint8_t) s[written];
        ++outputAvailable;
        ++written;
      }
      
      return written;
    }
};

#endif // ifndef
