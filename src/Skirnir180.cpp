#include "Skirnir180.hpp"
#include "base64.hpp"

Skirnir180::Skirnir180(HardwareSerial* port) : Skirnir(port) {
  receiveBuffer = getReceiveBuffer();
  receive_buffer[240] = '\0';
}

bool Skirnir180::fsmGlobals(uint8_t next) {
  switch(next) {
    case '&':
      fsmState = LPACKET_INTERMEDIATE;
      fsmRepeats = 0;
      return true;
    default:
      return Skirnir::fsmGlobals(next);
  }
}

uint8_t Skirnir180::fsmLocals(uint8_t next) {
  switch(fsmState) {
    case LPACKET_INTERMEDIATE:
      getReceiveBuffer()[fsmRepeats] = next;
      if(++fsmRepeats >= 240) fsmState = LPACKET_END;
      return 0;
    case LPACKET_END:
      fsmState = START;
      if(next == '\n') {
        decode_base64(getReceiveBuffer(), getReceiveBuffer());
        return 180;
      }
      return 0;
    default:
      return Skirnir::fsmLocals(next);
  }
}
