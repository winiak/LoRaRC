
#define RC_CHANNEL_MIN 990
#define RC_CHANNEL_MAX 2010

#define SBUS_MIN_OFFSET 173
#define SBUS_MID_OFFSET 992
#define SBUS_MAX_OFFSET 1811
#define SBUS_CHANNEL_NUMBER 16
#define SBUS_PACKET_LENGTH 25
#define SBUS_FRAME_HEADER 0x0f
#define SBUS_FRAME_FOOTER 0x00
#define SBUS_FRAME_FOOTER_V2 0x04
#define SBUS_STATE_FAILSAFE 0x08
#define SBUS_STATE_SIGNALLOSS 0x04
#define SBUS_UPDATE_RATE 15 //ms

#define SERVO_CHANNELS 8
unsigned int Servos[SERVO_CHANNELS] = {1500, 1500, 1000, 1500, 1500, 1500, 1500, 1500};  
#define SERVO_SHIFT 0
static unsigned long sbus_frame_period = 15000;
static unsigned long frame_time = 0;

void setup() {
  Serial.begin(100000, SERIAL_8E2);
  //Serial.begin(115200);
}

void loop() {
  if (micros() > frame_time) {
    servoTester();
    send_servo_frame();
    frame_time = micros() + sbus_frame_period;
  }
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

void send_servo_frame(){

    unsigned char packet[25];
    static int output[SBUS_CHANNEL_NUMBER] = {0};

    /*
     * Map 1000-2000 with middle at 1500 chanel values to
     * 173-1811 with middle at 992 S.BUS protocol requires
     */
    for (uint8_t i = 0; i < SBUS_CHANNEL_NUMBER; i++) {
        output[i] = map( (i < SERVO_CHANNELS ? (Servos[i] + SERVO_SHIFT) : 1500), RC_CHANNEL_MIN, RC_CHANNEL_MAX, SBUS_MIN_OFFSET, SBUS_MAX_OFFSET);
    }

    uint8_t stateByte = 0x00;
    /*
    if (lost_frames > 12) {
        stateByte |= SBUS_STATE_SIGNALLOSS;
    }
    if (failsafe_state) {
        stateByte |= SBUS_STATE_FAILSAFE;
    }
  */
    Serial.write(SBUS_FRAME_HEADER);
    Serial.write(output[0] & 0x07FF);
    Serial.write((output[0] & 0x07FF)>>8 | (output[1] & 0x07FF)<<3);

    Serial.write((output[1] & 0x07FF)>>5 | (output[2] & 0x07FF)<<6);
    Serial.write((output[2] & 0x07FF)>>2);
    Serial.write((output[2] & 0x07FF)>>10 | (output[3] & 0x07FF)<<1);
    Serial.write((output[3] & 0x07FF)>>7 | (output[4] & 0x07FF)<<4);
    Serial.write((output[4] & 0x07FF)>>4 | (output[5] & 0x07FF)<<7);
    Serial.write((output[5] & 0x07FF)>>1);
    Serial.write((output[5] & 0x07FF)>>9 | (output[6] & 0x07FF)<<2);
    Serial.write((output[6] & 0x07FF)>>6 | (output[7] & 0x07FF)<<5);
    Serial.write((output[7] & 0x07FF)>>3);
    Serial.write((output[8] & 0x07FF));
    Serial.write((output[8] & 0x07FF)>>8 | (output[9] & 0x07FF)<<3);
    Serial.write((output[9] & 0x07FF)>>5 | (output[10] & 0x07FF)<<6);  
    Serial.write((output[10] & 0x07FF)>>2);
    Serial.write((output[10] & 0x07FF)>>10 | (output[11] & 0x07FF)<<1);
    Serial.write((output[11] & 0x07FF)>>7 | (output[12] & 0x07FF)<<4);
    Serial.write((output[12] & 0x07FF)>>4 | (output[13] & 0x07FF)<<7);
    Serial.write((output[13] & 0x07FF)>>1);
    Serial.write((output[13] & 0x07FF)>>9 | (output[14] & 0x07FF)<<2);
    Serial.write((output[14] & 0x07FF)>>6 | (output[15] & 0x07FF)<<5);
    Serial.write((output[15] & 0x07FF)>>3);
    Serial.write (stateByte); //Flags byte
    Serial.write(SBUS_FRAME_FOOTER); //Footer
}

