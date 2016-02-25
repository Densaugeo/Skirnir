#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "HardwareSerial.h"

TEST_CASE("HardwareSerial()") {
  SECTION("Empty port is empty") {
    HardwareSerial port = HardwareSerial();
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 0);
  }
  
  SECTION("Constructor passes initial input") {
    HardwareSerial port = HardwareSerial(5, (uint8_t*) "asdf");
    
    REQUIRE(port.inputAvailable == 5);
    REQUIRE(memcmp(port.inputBuffer, "asdf", 5) == 0);
    REQUIRE(port.outputAvailable == 0);
  }
  
  SECTION("Initial input is truncated to 64 bytes") {
    HardwareSerial port = HardwareSerial(65, (uint8_t*) "asdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDF");
    
    REQUIRE(port.inputAvailable == 64);
    REQUIRE(memcmp(port.inputBuffer, "asdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDF", 64) == 0);
    REQUIRE(port.outputAvailable == 0);
  }
}

TEST_CASE("HardwareSerial.addInput()") {
  HardwareSerial port = HardwareSerial();
  
  SECTION("Adding zero bytes does nothing") {
    port.addInput(0, (uint8_t*) "foo");
    
    REQUIRE(port.inputAvailable == 0);
  }
  
  SECTION("Adding bytes puts them in .inputBuffer") {
    port.addInput(4, (uint8_t*) "foo");
    
    REQUIRE(port.inputAvailable == 4);
    REQUIRE(memcmp(port.inputBuffer, "foo", 4) == 0);
  }
  
  SECTION("Adding more bytes appends to .inputBuffer") {
    port.addInput(4, (uint8_t*) "foo");
    port.addInput(4, (uint8_t*) "foo");
    
    REQUIRE(port.inputAvailable == 8);
    REQUIRE(memcmp(port.inputBuffer, "foo\0foo", 8) == 0);
  }
  
  SECTION("New bytes are ignored after .inputBuffer is full") {
    port.addInput(10, (uint8_t*) "1234567890");
    port.addInput(65, (uint8_t*) "asdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDF");
    
    REQUIRE(port.inputAvailable == 64);
    REQUIRE(memcmp(port.inputBuffer, "1234567890asdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfAS", 64) == 0);
  }
}

TEST_CASE("HardwareSerial.available()") {
  HardwareSerial port = HardwareSerial();
  
  SECTION("Zero bytes available at start") {
    REQUIRE(port.available() == 0);
  }
  
  SECTION("Constucting with initial input makes it available") {
    HardwareSerial port_2 = HardwareSerial(4, (uint8_t*) "asdf");
    
    REQUIRE(port_2.available() == 4);
  }
  
  SECTION("Adding bytes makes them available") {
    port.addInput(4, (uint8_t*) "asdf");
    
    REQUIRE(port.available() == 4);
  }
  
  SECTION("Adding bytes successively makes them available") {
    port.addInput(4, (uint8_t*) "asdf");
    port.addInput(4, (uint8_t*) "asdf");
    
    REQUIRE(port.available() == 8);
  }
  
  SECTION("Reading bytes makes them not available any more") {
    port.addInput(4, (uint8_t*) "asdf");
    
    port.read();
    
    REQUIRE(port.available() == 3);
  }
  
  SECTION("Reading bytes successively makes them not available any more") {
    port.addInput(4, (uint8_t*) "asdf");
    
    port.read();
    port.read();
    
    REQUIRE(port.available() == 2);
  }
  
  SECTION("Reading from and empty port doesn't make availability negative") {
    port.read();
    
    REQUIRE(port.available() == 0);
  }
}

TEST_CASE("HardwareSerial.read()") {
  HardwareSerial port = HardwareSerial(4, (uint8_t*) "1337");
  
  SECTION("Reading an empty port returns -1") {
    HardwareSerial port_2 = HardwareSerial();
    
    REQUIRE(port_2.read() == -1);
  }
  
  SECTION("Reading a non-empty port returns the first byte") {
    REQUIRE(port.read() == '1');
  }
  
  SECTION("Reading a non-empty port removes the first byte from .inputBuffer") {
    port.read();
    
    REQUIRE(port.inputAvailable == 3);
    REQUIRE(memcmp(port.inputBuffer, "337", 3) == 0);
  }
  
  SECTION("Reading a non-empty port successively returns the first bytes") {
    REQUIRE(port.read() == '1');
    REQUIRE(port.read() == '3');
  }
  
  SECTION("Reading a non-empty port successively removes the first bytes from .inputBuffer") {
    port.read();
    port.read();
    
    REQUIRE(port.inputAvailable == 2);
    REQUIRE(memcmp(port.inputBuffer, "37", 2) == 0);
  }
}

TEST_CASE("HardwareSerial.write()") {
  HardwareSerial port = HardwareSerial();
  
  SECTION("Writing a byte puts it in .outputBuffer") {
    port.write('!');
    
    REQUIRE(port.outputAvailable == 1);
    REQUIRE(memcmp(port.outputBuffer, "!", 1) == 0);
  }
  
  SECTION("Writing bytes successively puts them in .outputBuffer") {
    port.write('!');
    port.write('@');
    
    REQUIRE(port.outputAvailable == 2);
    REQUIRE(memcmp(port.outputBuffer, "!@", 2) == 0);
  }
  
  SECTION("Writing bytes to a full buffer does nothing") {
    port.outputAvailable = 64;
    memcpy(port.outputBuffer, (uint8_t*) "1234567890asdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfAS", 64);
    
    port.write('!');
    port.write('@');
    
    REQUIRE(port.outputAvailable == 64);
    REQUIRE(memcmp(port.outputBuffer, "1234567890asdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfAS", 2) == 0);
  }
  
  SECTION("Writing a string puts several bytes in .outputBuffer") {
    REQUIRE(port.write("this is a string") == 16);
    
    REQUIRE(port.outputAvailable == 16);
    REQUIRE(memcmp(port.outputBuffer, "this is a string", 16) == 0);
  }
  
  SECTION("Writing string successively puts their bytes in .outputBuffer") {
    REQUIRE(port.write("this is a string") == 16);
    REQUIRE(port.write("this is another string") == 22);
    
    REQUIRE(port.outputAvailable == 38);
    REQUIRE(memcmp(port.outputBuffer, "this is a stringthis is another string", 38) == 0);
  }
  
  SECTION("Long strings are truncated when .outputBuffer is full") {
    REQUIRE(port.write("1234567890") == 10);
    REQUIRE(port.write("asdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDF") == 54);
    
    REQUIRE(port.outputAvailable == 64);
    REQUIRE(memcmp(port.outputBuffer, "1234567890asdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfASDFasdfAS", 64) == 0);
  }
}
