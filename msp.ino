#ifdef MSP_module

/**
 * This module is to provide bidirectional communication between RX and FC 
 * in Multiwii Serial Protocol standard.
 * The goal is to send RC values by MSP commands and get any needed data
 * out of FC and send it to the TX - this will provide full telemetry capability.
 * 
 * Due to the limitation of downlink (currenlty limited to 6 bytes, but 5 available), the downlink 
 * will be limited to few, selected paramaters (Voltage, Amps, GPS coords).
 * 
 * It is based on the MSP documentation available on Multiwii project WIKI:
 * http://www.multiwii.com/wiki/index.php?title=Multiwii_Serial_Protocol
 * 
 */
#ifdef RX_module

#include "msp.h"
#define MSP_MAXCHANNELS 8 // max 16 (or alawys 16?)

void setup_module() {
    // module works on hardware serial - shall be 57600 or 115200
}

void send_message(uint8_t messageID, void * payload, uint8_t messageSize) {
  uint8_t frame_size = 0;
  char frame_buffer[32];
  
  frame_buffer[0] = '$';
  frame_buffer[1] = 'M';
  frame_buffer[2] = '<';
  frame_buffer[3] = messageSize;
  frame_buffer[4] = messageID;
  
  uint8_t checksum = frame_size ^ messageID;
  uint8_t * payloadPtr = (uint8_t*)payload;

  for (uint8_t i = 5; i < messageSize + 5 ; ++i) {
    uint8_t b = *(payloadPtr++);
    checksum ^= b;
    frame_buffer[i] = b;
  }
  
  frame_buffer[messageSize + 5] = checksum;
  
  Serial.write(frame_buffer);
}

void send_request(uint8_t messageID) {
  uint8_t frame_size = 0;
  uint8_t checksum = frame_size ^ messageID;
  char frame_buffer[32];
  
  frame_buffer[0] = '$';
  frame_buffer[1] = 'M';
  frame_buffer[2] = '<';
  frame_buffer[3] = '0';
  frame_buffer[4] = messageID;
  frame_buffer[5] = checksum;

  Serial.write(frame_buffer);
}

void send_servo_frame() {
  uint8_t frame_size = MSP_MAXCHANNELS * 2;
  uint8_t messageID = MSP_RC;     // Send RC data message 
  uint8_t checksum = frame_size ^ messageID;
  char frame_buffer[32];
  unsigned int temp_servo, temp;
  uint8_t i;
  
  frame_buffer[0] = '$';
  frame_buffer[1] = 'M';
  frame_buffer[2] = '<';
  frame_buffer[3] = frame_size; //frame_size
  frame_buffer[4] = messageID;

  // Build frame
  for (i = 5; i < MSP_MAXCHANNELS + 5; i++) {
    temp_servo = (i < SERVO_CHANNELS ? (Servos[i] + SERVO_SHIFT)/2 : 1500);   // add shift (config) and make compatible (divide by 2)
    frame_buffer[(i * 2) + 2] =  temp_servo % 256;    
    checksum ^= (temp_servo % 256);
    frame_buffer[(i * 2) + 3] =  temp_servo / 256;
    checksum ^= (temp_servo / 256);
  }
  
  // Add CRC to frame
  frame_buffer[MSP_MAXCHANNELS * 2 + 5];

  // Send to serial (soft serial does not work at 115200) - HW UART MUST BE USED
  Serial.println(frame_buffer);
}

#endif // RX_module

#endif // MSP_module
