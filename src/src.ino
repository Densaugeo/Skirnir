// This is a small test script for testing Skirnir on hardware

#include "Skirnir180.hpp"

uint16_t last_ping = 0;

Skirnir a_skirnir = Skirnir(&Serial);

void setup() {
  Serial.begin(115200);
}

void loop() {
  // Start heartbeat every 2s
  if((uint16_t) millis() - last_ping > 2000) {
    last_ping = millis();

    a_skirnir.heartbeat();
  }

  // Returns true if a valid packet is found
  switch(a_skirnir.receiveUntilPacket()) {
    // Echo packet back to PC
    case 45:
      a_skirnir.send(a_skirnir.receiveBuffer, 45);
      break;
    case 180:
      a_skirnir.send(a_skirnir.receiveBuffer, 180);
      break;
  }
}
