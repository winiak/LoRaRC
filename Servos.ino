/*
 * Servo functions used in project.
 * 
 * Thanks to Zendes for SBUS encoding/decoding code https://github.com/zendes/SBUS
 */

char buildServoData() 
{
  // use SBUS compression method
  TX_Buffer[0] = "S";  
  TX_Buffer[1] = (uint8_t) ((Servos[0] & 0x07FF));
  TX_Buffer[2] = (uint8_t) ((Servos[0] & 0x07FF)>>8 | (Servos[1] & 0x07FF)<<3);
  TX_Buffer[3] = (uint8_t) ((Servos[1] & 0x07FF)>>5 | (Servos[2] & 0x07FF)<<6);
  TX_Buffer[4] = (uint8_t) ((Servos[2] & 0x07FF)>>2);
  TX_Buffer[5] = (uint8_t) ((Servos[2] & 0x07FF)>>10 | (Servos[3] & 0x07FF)<<1);
  TX_Buffer[6] = (uint8_t) ((Servos[3] & 0x07FF)>>7 | (Servos[4] & 0x07FF)<<4);
  TX_Buffer[7] = (uint8_t) ((Servos[4] & 0x07FF)>>4 | (Servos[5] & 0x07FF)<<7);
  TX_Buffer[8] = (uint8_t) ((Servos[5] & 0x07FF)>>1);
  TX_Buffer[9] = (uint8_t) ((Servos[5] & 0x07FF)>>9 | (Servos[6] & 0x07FF)<<2);
  TX_Buffer[10] = (uint8_t) ((Servos[6] & 0x07FF)>>6 | (Servos[7] & 0x07FF)<<5);
  TX_Buffer[11] = (uint8_t) ((Servos[7] & 0x07FF)>>3);
  return 12;
}

void decodeServoData() {
  char i = 1;
  Servo_Buffer[0]  = (uint16_t) ((RX_Buffer[i+0]    |RX_Buffer[i+1] <<8)                     & 0x07FF);
  Servo_Buffer[1]  = (uint16_t) ((RX_Buffer[i+1]>>3 |RX_Buffer[i+2] <<5)                     & 0x07FF);
  Servo_Buffer[2]  = (uint16_t) ((RX_Buffer[i+2]>>6 |RX_Buffer[i+3] <<2 |RX_Buffer[i+4]<<10)    & 0x07FF);
  Servo_Buffer[3]  = (uint16_t) ((RX_Buffer[i+4]>>1 |RX_Buffer[i+5] <<7)                     & 0x07FF);
  Servo_Buffer[4]  = (uint16_t) ((RX_Buffer[i+5]>>4 |RX_Buffer[i+6] <<4)                     & 0x07FF);
  Servo_Buffer[5]  = (uint16_t) ((RX_Buffer[i+6]>>7 |RX_Buffer[i+7] <<1 |RX_Buffer[i+8]<<9)     & 0x07FF);
  Servo_Buffer[6]  = (uint16_t) ((RX_Buffer[i+8]>>2 |RX_Buffer[i+9] <<6)                     & 0x07FF);
  Servo_Buffer[7]  = (uint16_t) ((RX_Buffer[i+9]>>5 |RX_Buffer[i+10] <<3) & 0x07FF);
}

void servoTester() {
  static unsigned int swipeval = 1000;
  static char sign = 1;
  static unsigned int sstep = 20;
  
  if (swipeval <= 1000)
    sign = 1;
  if (swipeval >= 2000)
    sign = -1;
  swipeval = swipeval + sstep * sign;

  for (char i=0; i<8; i++)
    Servos[i] = (i != 2 ? swipeval : Servos[i]);
}

