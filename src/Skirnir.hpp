/**
 * Arduino side of Skirnir. Responsible for handling serial packets
 */
#ifndef SKIRNIR_H_INCLUDED
#define SKIRNIR_H_INCLUDED

#include <HardwareSerial.h>

class Skirnir {
  private:
    // Where raw packets are stored as they come in, and where finished packets are put when they are found
    uint8_t receiveBufferPrivate[61];
  
  protected:
    enum FSMState {
      START,
      PING,
      PACKET_INTERMEDIATE,
      PACKET_END,
      LPACKET_INTERMEDIATE,
      LPACKET_END
    };
    
    HardwareSerial* port;
    FSMState fsmState;
    int fsmRepeats;
    
    /* fsmGlobals:
     *   Description:
     *     First stage of FSM used by receive()
     *   Parameters:
     *     next - The character to receive
     *   Returns:
     *     true if next character triggered a global transition
     */
    virtual bool fsmGlobals(uint8_t next);
    
    /* fsmLocals:
     *   Description:
     *     Second stage of FSM used by receive()
     *   Parameters:
     *     next - The character to receive
     *   Returns:
     *     if a packet was completed, size in bytes (always 45). Otherwise, zero
     */
    virtual uint8_t fsmLocals(uint8_t next);
    
    // Get pointer to correct receive buffer
    virtual uint8_t* getReceiveBuffer() { return receiveBufferPrivate; }
  
  public:
    // Publicly accessible pointer to the receive buffer
    uint8_t* receiveBuffer;
    
    /* Skirnir:
     *   Description:
     *     Sends and receives 45-byte packets over an Arduino-compatible HardwareSerial port. Can send 180-byte packets,
     *     but can only receive 45-byte packets
     *   Parameters:
     *     port - Reference to an Arduino-style HardwareSerial
     *   Returns:
     *     A Skirnir instance that is ready to use
     */
    Skirnir(HardwareSerial* port);
    
    /* heartbeat:
     *   Description:
     *     Initiates a heartbeat. Heartbeats must occur regularly; if the host PC does not detect a heartbeat for
     *     7 seconds, it will report a connection error
     */
    void heartbeat();
    
    /* send:
     *   Description:
     *     Sends a packet, with a payload of up to 180 bytes
     *     Smaller payloads are sent in 45-byte packets, larger ones in 180-byte packets
     *   Parameters:
     *     payload - Bytes to send
     *     size - Size of payload. If size is more than 180, the first 180 bytes will be sent
     */
    void send(uint8_t payload[], uint32_t size);
    
    /* receive:
     *   Description:
     *     Recieves a character. Responds to heartbeats and receives packets, if detected. If a packet is found,
     *     it is parsed and left in .receiveBuffer. Received packets are overwritten by .receive() and
     *     .receiveUntilPacket()
     *   Parameters:
     *     next - The character to receive
     *   Returns:
     *     if a packet was detected, size in bytes (always 45). Otherwise, zero
     */
    uint8_t receive(uint8_t next);
    
    /* receiveUntilPacket:
     *   Description:
     *     Recieves from port.read() until a packet is detected or input ends. Responds to heartbeats, if detected.
     *     If a packet is found, it is parsed and left in .receiveBuffer. Received packets are overwritten by
     *     .receive() and .receiveUntilPacket()
     *   Returns:
     *     if a packet was detected, size in bytes (always 45). Otherwise, zero
     */
    uint8_t receiveUntilPacket();
};

#endif // ifndef
