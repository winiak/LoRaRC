//#define TEST_TX_MODULE
//#define TEST_RX_MODULE

#ifdef TEST_TX_MODULE
  #define DEBUG_ANALYZER
  //#define DEBUG_RADIO_EXCH
  //#define DEBUG_CH_FREQ
  #define TX_SERVO_TESTER
  #define TX_module
  #define PPM_module
#endif

#ifdef TEST_RX_MODULE
  #define RX_module
  #define DEBUG_ANALYZER
#endif

// Dubugging - select any
//#define DEBUG_CH_FREQ
//#define DEBUG_RADIO_EXCH
//#define DEBUG_ANALYZER
//#define TX_SERVO_TESTER
//#define DEBUG_RX_OUTPUT

// Transmitter or Receiver - select one
#define TX_module
//#define RX_module

// Communication type - select one
//#define PPM_module  // using ICP for TX or declared for TX
//#define IBUS_module   // using UART
//#define SBUS_module   // using UART at 100000N2
//#define MSP_module   // using UART

// Transmitting power in dBm: 2 to 20, default 17
byte tx_power_low = 4;    //  4=>2.5mW; 6=>5mW; 10=>10mW
byte tx_power_high = 10;  //  12=>16mW; 14=>25mW; 16=>40mW; 18=>63mW; 20=>100mW

int power_thr_high = -90;  // testing: 190 (-65); flying:  180 (-75)
int power_thr_low = -100;   // testing: 180 (-75); flying:  160 (-95)
byte tx_power_step = 2;
#define TX_POWER_DELAY_FILTER sizeof(hop_list)*2

// Signal bandwidth and frame time 
// full range 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3, 500E3.
// measured (16Bytes from Transmitter, 6Bytes from Receiver : 
//    SF6,BW500 => 12ms; SF6,BW250 => 22ms; SF6,BW125 => 43ms; SF6,BW62,5 => 83ms
unsigned long BW_low = 125E3;   //Hz
unsigned long BW_high = 250E3;  //Hz
unsigned long F_rate_low = 45000;  //us
unsigned long F_rate_high = 22000; //us
byte spread_factor = 6;

#define base_frequency  868100000
#define frequency_step  100000
const uint8_t hop_list[] = {5,7,12};
#define RX_FAST_HOP_TIME_MUL 1.2
#define RX_LOST_HOP_TIME_MUL 3
#define LOST_FRAMES_COUNT 30  // sizeof(hop_list) * 10


// Servos & channels
#define SERVO_CHANNELS 8
volatile unsigned int Servo_Buffer[SERVO_CHANNELS] = {1500, 1500, 1000, 1500, 1500, 1500, 1500, 1500};
unsigned int Servos[SERVO_CHANNELS] = {1500, 1500, 1000, 1500, 1500, 1500, 1500, 1500};  
unsigned int Servo_Failsafe[SERVO_CHANNELS] = {1500, 1500, 900, 1500, 1500, 1500, 1500, 150};
#define FAILSAFE_DELAY_MS 800

#define SERVO_SHIFT 0   //(-16)   // PPM = -16  |  others = 0

// inject RSSI signal into Channel (CH1 = 0)
#define INJECT_RSSI_IN_CH 7

// PPM 
#define PPM_OUT 8   
#define PPM_OUT_HIGH PORTB |= _BV(0)
#define PPM_OUT_LOW PORTB &= ~_BV(0)
#define PPM_IN  8 // ICP1

// RFM95 to Arduino HW connection
#define NSS_PIN 10
#define NRESET_PIN A0
#define DIO0_PIN 2  // must be IRQ assignable
