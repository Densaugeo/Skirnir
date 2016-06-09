CXX ?= g++
CXXFLAGS ?= -Wall -std=c++11 -I test -I src

test: test/HardwareSerial_test test/Skirnir_test test/Skirnir180_test

test/HardwareSerial_test: test/HardwareSerial_test.cpp test/HardwareSerial.h test/catch.hpp
	$(CXX) $(CXXFLAGS) test/HardwareSerial_test.cpp -o $@
	./$@

test/Skirnir_test: test/Skirnir_test.cpp src/Skirnir.hpp src/base64.hpp test/HardwareSerial_test test/catch.hpp
	$(CXX) $(CXXFLAGS) test/Skirnir_test.cpp -o $@
	./$@

test/Skirnir180_test: test/Skirnir180_test.cpp src/Skirnir180.hpp test/Skirnir_test test/HardwareSerial_test test/catch.hpp
	$(CXX) $(CXXFLAGS) test/Skirnir180_test.cpp -o $@
	./$@

clean:
	rm test/HardwareSerial_test test/Skirnir_test test/Skirnir180_test
