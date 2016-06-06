// This is a small test script for testing Skirnir on hardware

#include "Skirnir180.hpp"

unsigned char packet_decoded[180];
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
  switch(a_skirnir.receive_until_packet(packet_decoded)) {
    // Echo packet back to PC
    case 45:
      a_skirnir.send(packet_decoded, 45);
      break;
    case 180:
      a_skirnir.send(packet_decoded, 180);
      break;
  }
}
