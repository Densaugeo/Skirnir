CXX ?= g++
CXXFLAGS ?= -Wall -std=c++11 -I test -I src 

test: test/HardwareSerial_test test/Skirnir_test

test/HardwareSerial_test: test/HardwareSerial_test.cpp test/HardwareSerial.cpp test/HardwareSerial.h test/catch.hpp
	$(CXX) $(CXXFLAGS) test/HardwareSerial_test.cpp test/HardwareSerial.cpp  -o $@
	./test/HardwareSerial_test

test/Skirnir_test: test/Skirnir_test.cpp src/Skirnir.cpp src/Skirnir.hpp src/base64.cpp src/base64.hpp test/HardwareSerial_test test/catch.hpp
	$(CXX) $(CXXFLAGS) test/Skirnir_test.cpp src/Skirnir.cpp src/base64.cpp test/HardwareSerial.cpp -o $@
	./test/Skirnir_test

clean:
	rm test/HardwareSerial_test test/Skirnir_test
