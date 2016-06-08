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
    /* fsmGlobals:
     *   Description:
     *     Same as Skirnir::fsmGlobals(), but also handles 180-byte packets
     */
    virtual bool fsmGlobals(uint8_t next);
    
    /* fsmLocals:
     *   Description:
     *     Same as Skirnir::fsmLocals(), but also handles 180-byte packets
     */
    virtual uint8_t fsmLocals(uint8_t next);
  
  public:
    // Publicly accessible pointer to the receive buffer
    virtual uint8_t* getReceiveBuffer() { return receive_buffer; }
    
    /* Skirnir180:
     *   Description:
     *     Same as Skirnir, but can receive 180-byte packets (at the cost of using more RAM)
     */
    Skirnir180(HardwareSerial* port);
};

#endif // ifndef
