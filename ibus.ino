#ifdef IBUS_module

/**
 * iBus protocol generator by Konrad Winiarski
 * - why iBus? Latency! Frame sent to FC every 7ms!
 * - 14 channels by default !!
 * - support bi-directional communication (just in case)
 * - consumes UART, SoftSerial does not work at 115200 :(
 * 
 * Frame example:
 * x20x40   = Header (length of 32 bytes + command)
 * xDCx05  = 1500 ch 1
 * xDBx05  = 1499 ch 2
 * xEFx03  = 1007 ch 3
 * xDDx05  = 1501 ch 4
 * xD0x07  = 2000 ch 5
 * xD0x07  = 2000 ch 6
 * xDCx05  = 1500 ch 7
 * xDCx05  = 1500 ch 8
 * xDCx05  = 1500 ch 9
 * xDCx05  = 1500 ch 10
 * xDCx05  = 1500 ch 11
 * xDCx05  = 1500 ch 12
 * xDCx05  = 1500 ch 13
 * xDCx05  = 1500 ch 14
 * x54xF3  = Checksum:  0xFFFF - (0x20 + 0x40 ... sum of all !)
 * 
 * Thanks to Basejunction for introduction :)
 * https://basejunction.wordpress.com/2015/08/23/en-flysky-i6-14-channels-part1/
 */

#ifdef RX_module

#define IBUS_MAXCHANNELS 14

void setup_module() {
    // module works on hardware serial - must be 115200
}

void send_frame() {
  static long ibus_frame_timer;
  if (failsafe_state) {
    Serial.println("Failsafe");
    return;
  }
  char frame_buffer[32];
  char ptr, len;
  unsigned int chksum = 65535 - 32 - 64; //; // - 0x20  - 0x40; // 0x0000  - 0x20 - 0x40;
  unsigned int temp_servo, temp;

  frame_buffer[0] = 0x20;   // frame length = 32 bytes 
  frame_buffer[1] = 0x40;   // command - servo data

  // Build frame
  for (byte i = 0; i < IBUS_MAXCHANNELS; i++) {
    temp_servo = (i < SERVO_CHANNELS ? (Servos[i] + SERVO_SHIFT) : 1500);   // add shift (config)
    frame_buffer[(i * 2) + 2] =  temp_servo % 256;    
    chksum -= (temp_servo % 256);
    frame_buffer[(i * 2) + 3] =  temp_servo / 256;
    chksum -= (temp_servo / 256);
  }

  // Add CRC to frame
  frame_buffer[30] = chksum % 256;
  frame_buffer[31] = chksum / 256;

  // Send to serial (soft serial does not work at 115200) - HW UART MUST BE USED
  Serial.write(frame_buffer);
 
  
  #ifdef DEBUG_RX_OUTPUT
    // Show all data in ASCII 
    PrintHex8(frame_buffer, 32);
    Serial.println();
  
  /*
    for (byte i = 0; i < 8; i++)
    {
      Serial.print((Servos[i] + SERVO_SHIFT) / 2);
      Serial.print(" ");
    }
    Serial.println();
    */
  #endif // DEBUG_RX_OUTPUT
}


#endif //RX_module
#endif // IBUS
