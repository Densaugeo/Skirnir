/**
 * Arduino side of Skirnir. Responsible for handling serial packets
 */
#ifndef SKIRNIR180_H_INCLUDED
#define SKIRNIR180_H_INCLUDED

#include <HardwareSerial.h>
#include "Skirnir.hpp"

class Skirnir180 : public Skirnir {
  private:
    uint8_t receive_buffer[241];
  
  protected:
    bool fsmGlobals(uint8_t payload[], uint8_t next, uint8_t input_buffer[]);
    uint8_t fsmLocals(uint8_t payload[], uint8_t next, uint8_t input_buffer[]);
  
  public:
    Skirnir180(HardwareSerial* port);
    
    /* send:
     *   Description:
     *     Sends a packet, with a payload of up to 190 bytes. Packets larger than 45 bytes are split into up to
     *     4 frames
     *   Parameters:
     *     payload - Bytes to send
     *     size - Size of payload. If size is more that 190, only 190 bytes will be sent
     */
    void send(uint8_t payload[], uint32_t size);
    uint8_t receive(uint8_t payload[], uint8_t next);
    uint8_t receive_until_packet(uint8_t payload[]);
    void send180(uint8_t payload[]);
    
};

#endif // ifndef
