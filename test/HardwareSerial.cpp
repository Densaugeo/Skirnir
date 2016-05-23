#include "HardwareSerial.h"

HardwareSerial::HardwareSerial() {
  inputAvailable = 0;
  outputAvailable = 0;
}

HardwareSerial::HardwareSerial(uint8_t payload[], uint32_t size) {
  inputAvailable = 0;
  outputAvailable = 0;
  
  addInput(payload, size);
}

void HardwareSerial::addInput(uint8_t payload[], uint32_t size) {
  uint32_t size_accepted = size < HARDWARESERIAL_INPUT_SIZE - inputAvailable ? size : HARDWARESERIAL_INPUT_SIZE - inputAvailable;
  
  for(uint32_t i = 0; i < size_accepted; ++i) {
    inputBuffer[inputAvailable + i] = payload[i];
  }
  
  inputAvailable += size_accepted;
}

int HardwareSerial::available() {
  return (int) inputAvailable;
}

int HardwareSerial::read() {
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

size_t HardwareSerial::write(uint8_t c) {
  if(outputAvailable < HARDWARESERIAL_OUTPUT_SIZE) {
    outputBuffer[outputAvailable] = c;
    ++outputAvailable;
    
    return 1;
  }
  
  return 0;
}

size_t HardwareSerial::write(const char s[]) {
  size_t written = 0;
  
  while(outputAvailable < HARDWARESERIAL_OUTPUT_SIZE && s[written] != '\0') {
    outputBuffer[outputAvailable] = (uint8_t) s[written];
    ++outputAvailable;
    ++written;
  }
  
  return written;
}
