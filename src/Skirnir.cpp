#include "Skirnir.hpp"
#include "base64.hpp"

Skirnir::Skirnir(HardwareSerial* port_) {
  port = port_;
  fsm_state = START;
  receiveBuffer = getReceiveBuffer();
}

void Skirnir::heartbeat() {
  port -> write("<\n");
}

void Skirnir::send(uint8_t payload[], uint32_t size) {
  uint8_t payload_size = size < 180 ? size : 180;
  uint8_t packet_size = size <= 45 ? 60 : 240;
  
  uint8_t send_buffer[5];
  send_buffer[4] = '\0';
  
  // Skirnir packet prefix
  port -> write(size <= 45 ? '#' : '&');
  
  // Base64-encode payload in blocks of 3
  for(uint8_t i = 0; i < payload_size - 2; i += 3) {
    encode_base64(payload + i, 3, send_buffer);
    port -> write((char*) send_buffer);
  }
  
  // Check for a partial block at end of payload
  if(payload_size % 3 != 0) {
    send_buffer[1] = payload[payload_size/3*3];
    send_buffer[2] = payload_size % 3 == 2 ? payload[payload_size/3*3 + 1] : 0;
    send_buffer[3] = 0;
    
    encode_base64(send_buffer + 1, 3, send_buffer);
    port -> write((char*) send_buffer);
  }
  
  // Fill remaineder of packet with base64 zeros ('A' characters)
  for(uint8_t i = (payload_size + 2)/3*4; i < packet_size; ++i) {
    port -> write('A');
  }
  
  port -> write('\n');
}

bool Skirnir::fsmGlobals(uint8_t next) {
  switch(next) {
    case '-':
      fsm_state = PING;
      return true;
    case '#':
      fsm_state = PACKET_INTERMEDIATE;
      fsm_repeats = 0;
      getReceiveBuffer()[60] = '\0';
      return true;
    default:
      return false;
  }
}

uint8_t Skirnir::fsmLocals(uint8_t next) {
  switch(fsm_state) {
    case PING:
      if(next == '\n') port -> write(">\n");
      fsm_state = START;
      return 0;
    case PACKET_INTERMEDIATE:
      getReceiveBuffer()[fsm_repeats] = next;
      if(++fsm_repeats >= 60) fsm_state = PACKET_END;
      return 0;
    case PACKET_END:
      fsm_state = START;
      if(next == '\n') {
        decode_base64(getReceiveBuffer(), getReceiveBuffer());
        return 45;
      }
      return 0;
    default:
      return 0;
  }
}

uint8_t Skirnir::receive(uint8_t next) {
  if(fsmGlobals(next)) {
    return 0;
  } else {
    return fsmLocals(next);
  }
}

uint8_t Skirnir::receive_until_packet() {
  uint8_t result = 0;
  
  while(port -> available()) {
    result = receive(port -> read());
    
    if(result) {
      return result;
    }
  }
  
  return 0;
}
