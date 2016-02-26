# Skirnir
Link Arduino to Nodejs, reliably

## Installation

#### Arduino

Add contents of src/ to your project folder and `#include "Skirnir.hpp" in your main .cpp file.

## Usage

#### Arduino

The following minimal code can maintain a connection and echo received packets back to the PC:

~~~
#include "Skirnir.hpp"

unsigned char packet_decoded[45];
unsigned long last_ping = 0;

Skirnir a_skirnir = Skirnir(&Serial);

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Check timer for overflow
  if(millis() < last_ping) {
    last_ping = 0;
  }
  
  // Start heartbeat every 5s
  if(millis() > last_ping + 5000) {
    last_ping = millis();
    
    a_skirnir.heartbeat();
  }
  
  // Returns true if a valid packet is found
  if(a_skirnir.receive_until_packet(packet_decoded)) {
    // Echo packet back to PC
    a_skirnir.send45(packet_decoded);
  }
}
~~~

## License

LGPL
