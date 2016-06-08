#include "Skirnir180.hpp"
#include "base64.hpp"

Skirnir180::Skirnir180(HardwareSerial* port_) : Skirnir(port) {
  port = port_;
  fsm_state = START;
  receive_buffer[240] = '\0';
  receiveBuffer = getReceiveBuffer();
}

bool Skirnir180::fsmGlobals(uint8_t next) {
  switch(next) {
    case '&':
      fsm_state = LPACKET_INTERMEDIATE;
      fsm_repeats = 0;
      return true;
    default:
      return Skirnir::fsmGlobals(next);
  }
}

uint8_t Skirnir180::fsmLocals(uint8_t next) {
  switch(fsm_state) {
    case LPACKET_INTERMEDIATE:
      getReceiveBuffer()[fsm_repeats] = next;
      if(++fsm_repeats >= 240) fsm_state = LPACKET_END;
      return 0;
    case LPACKET_END:
      fsm_state = START;
      if(next == '\n') {
        decode_base64(getReceiveBuffer(), getReceiveBuffer());
        return 180;
      }
      return 0;
    default:
      return Skirnir::fsmLocals(next);
  }
}
