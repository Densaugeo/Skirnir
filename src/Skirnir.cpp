#include "Skirnir.hpp"
#include "base64.hpp"

#define START 0
#define PING_I 1
#define PACKET_START 2
#define PACKET_END 3

unsigned char packet[66];
int packet_cursor;
int state = START;

Skirnir::Skirnir(HardwareSerial* port_) {
  port = port_;
}

void Skirnir::heartbeat() {
  port -> write("<\n");
}

void Skirnir::send45(unsigned char payload[]) {
  unsigned char packet[63];
  
  packet[0] = '#';
  encode_base64(payload, 45, packet + 1);
  packet[61] = '\n';
  packet[62] = '\0';
  
  port -> write((char*) packet);
}

void Skirnir::send(unsigned char payload[], unsigned int size) {
  int actual_size = size < 45 ? size : 45;
  unsigned char padded_payload[45];
  
  for(int i = 0; i < actual_size; ++i) {
    padded_payload[i] = payload[i];
  }
  
  for(int i = actual_size; i < 45; ++i) {
    padded_payload[i] = 0;
  }
  
  send45(padded_payload);
}

bool Skirnir::receive(unsigned char payload[], unsigned char next) {
  switch(next) {
    case '-':
      state = PING_I;
      break;
    case '#':
      for(int i = 0; i < 66; ++i) {
        packet[i] = '\0';
      }
      packet_cursor = 0;
      state = PACKET_START;
      break;
    default:
      switch(state) {
        case PING_I:
          if(next == '\n') port -> write(">\n");
          state = START;
          break;
        case PACKET_START:
          packet[packet_cursor++] = next;
          if(packet_cursor >= 60) state = PACKET_END;
          break;
        case PACKET_END:
          state = START;
          if(next == '\n') {
            decode_base64(packet, payload);
            return true;
          }
          break;
      }
  }
  
  return false;
}

bool Skirnir::receive_until_packet(unsigned char payload[]) {
  while(port -> available()) {
    char next = port -> read();
    
    if(receive(payload, next)) {
      return true;
    }
  }
  
  return false;
}
