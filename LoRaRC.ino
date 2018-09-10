/**
 * LoRaLRS project by Konrad Winiarski
 * 
 * Using LoRa library developed by Sandeep Mistry: https://github.com/sandeepmistry/arduino-LoRa
 * Special thanks to Jacek S. for inspiration ;)
 * RTF95 documentation: http://www.hoperf.com/upload/rf/RFM95_96_97_98W.pdf
 * 
 */

#include <SPI.h>
#include <LoRa.h>
#include "config.h"
#include <avr/wdt.h>


  #ifdef TX_module
    stateMachineDef stateMachine = TRANSMIT;
  #endif
  #ifdef RX_module
    stateMachineDef stateMachine = RECEIVE;
  #endif
  static unsigned long TX_period = F_rate_low;  // 7700 / 20000 / 40000  us
  static unsigned long ibus_frame_period = 7500; //us (so far the smallest delays with 7,5ms, measured frame rate 7,7 to 8,5ms
  static unsigned long RX_last_frame_received, RX_hopping_time;
  static unsigned long RX_frame_time;
  static unsigned long timer_start, timer_stop;

  unsigned long lost_frames = 0;
  uint8_t current_channel = 0;
  byte current_power = tx_power_low;
  byte power_delay_counter = TX_POWER_DELAY_FILTER;
  unsigned char TX_Buffer[12];
  unsigned char TX_Buffer_Len = 0;
  unsigned char RX_Buffer[12];
  unsigned char RX_Buffer_Len = 0;
  int RX_RSSI;  // RSSI on receiver side
  int TX_RSSI;  // RSSI on transmitter side
  static uint8_t calculated_rssi = 0;
  static uint8_t calculated_lost_frames_rssi = 0;
  int counter = 0;
  
  static bool failsafe_state = true;    // sending Failsafe values by default
  
void setup() {
  Serial.begin(115200);
  // while (!Serial);
  wdt_enable(WDTO_250MS);
  
  Serial.println("LoRa for Remote Control");
/*
  LoRa.setSignalBandwidth(125E3); // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, and 250E3.
  LoRa.setSpreadingFactor(7); // 6 to 12 - 6 requires IMPLICIT
  LoRa.setCodingRate4(5);   // 4 to 8, default 5
  LoRa.enableCrc(); // LoRa.disableCrc();
  LoRa.setTxPower(5); // 2 to 20, default 17

*/
  LoRa.setPins(NSS_PIN, NRESET_PIN, DIO0_PIN);   //(ss, reset, dio0) dio is optional but must be interrupt capable via attachInterrupt(...): prefered: nSS-10, nRESET-9, DI0-2
  if (!LoRa.begin(base_frequency + (hop_list[0] * 50000))) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setTxPower(tx_power_low); 
  LoRa.setSpreadingFactor(spread_factor); // 6 to 12 - 6 requires IMPLICIT
  
  LoRa.setSignalBandwidth(BW_high); 
  TX_period = F_rate_high;
  
  LoRa.enableCrc();
  //LoRa.disableCrc();
  wdt_reset();
  #ifdef DEBUG_ANALYZER
    #ifdef TX_module
    Serial.println("HC\tTX RSSI\tRX RSSI\tLost\tPacket t[ms]\tTX Pwr[dBm]\tRX Pwr[dBm]\tInput");
    #endif //TX_module
  #endif // DEBUG_ANALYZER

  #ifdef TX_module
    #ifdef PPM_module
      setup_module();
    #endif
  #endif
}

void loop() {
  static long int packet_timer;
  static unsigned long transmit_time = 0;
  static unsigned long receive_time = 0;
  static unsigned long twenty_ms_time = 0;
  static unsigned long ibus_frame_time = 0;
  static uint8_t no_RX_ack = 0; 


/**
 * Transmitter
 */
#ifdef TX_module
  switch (stateMachine) {
    case TRANSMIT:    // transmit data then switch to RECEIVE
      #ifdef TX_SERVO_TESTER
        servoTester();                    // servo tester - change servo values continuously
      #endif
      #if defined(DEBUG_RADIO_EXCH) || defined(DEBUG_CH_FREQ) || defined(DEBUG_ANALYZER) || defined(PPM_module)
      Serial.println();
      #endif
      Hopping();
      if (no_RX_ack > 0) {                // ACKonwlege, check for return of the telemetry data
        #ifdef DEBUG_RADIO_EXCH
        Serial.print("\t\t\t NO ACK: ");
        Serial.print(no_RX_ack);
        #endif
        if (no_RX_ack > TX_POWER_DELAY_FILTER) {             // if no ACK for (TX_POWER_DELAY_FILTER - two times hopping) frames, then assume no telemetry (and RSSI) is available
          TX_RSSI = -254;
          RX_RSSI = -254;
          if (no_RX_ack % TX_POWER_DELAY_FILTER * current_power * current_power == 0)     // the higher power is set, the longer time is needed to increase power by the STEP
            power_increase();
        }
      }
      TX_Buffer_Len = buildServoData();
      packet_timer = micros();
      sendBufferData();
      //Serial.print(micros() - packet_timer); 
      stateMachine = RECEIVE;
      no_RX_ack++;
      break;
      
    case RECEIVE:     // stay in RECEIVE and wait for data until next TX period
      if (receiveData(6)) {
        no_RX_ack = 0;
        TX_RSSI = RX_Buffer[0] - 255;// - 255; "0" to display byte values instead of dBm
        #ifdef DEBUG_CH_FREQ
        Serial.print("\tHC: ");
        Serial.print(current_channel);
        #endif
        #ifdef DEBUG_RADIO_EXCH
          Serial.print("\tTr RSSI: ");
          Serial.print(TX_RSSI);
          Serial.print("\tRe RSSI: ");
          Serial.print(RX_RSSI);
          Serial.print("\tPacket exchange time: ");
          Serial.print(micros() - packet_timer);
        #endif
        if (TX_RSSI < power_thr_low && power_delay_counter-- == 0) {
          power_increase();
          power_delay_counter = TX_POWER_DELAY_FILTER;
        }
        if (TX_RSSI > power_thr_high && power_delay_counter-- == 0) {
          power_decrease();
          power_delay_counter = TX_POWER_DELAY_FILTER;
        }
      }
      break;
    case SETUP:
      break;
    default: // controller
      break;
  }
  // State machine controller
  if (micros() > transmit_time) {
    #ifdef DEBUG_ANALYZER
      //Serial.println("TX RSSI\tRX RSSI\tLost");
      Serial.print(current_channel); Serial.print("\t");
      Serial.print(TX_RSSI); Serial.print("\t"); 
      Serial.print(RX_RSSI); Serial.print("\t"); 
      Serial.print(no_RX_ack); Serial.print("\t"); 
      Serial.print(byte((micros() - packet_timer)/1000)); Serial.print("\t");
      Serial.print(current_power); Serial.print("\t"); 
      Serial.print(RX_Buffer[1]); 
      #ifdef PPM_module      
        Serial.print("\t"); Serial.print(check_PPM_corrupted() ? "0" : "111");   
      #endif
    #endif
    transmit_time = micros() + TX_period;
    timer_start = micros();
    stateMachine = TRANSMIT;
  }

#endif // TX_module

/**
 * Receiver
 */
#ifdef RX_module
  TX_Buffer_Len = 6;
  RX_Buffer_Len = 12;
    //  last_frame_received
    //  TX_period
    //  lost_frames
  
  switch (stateMachine) {
    case RECEIVE:
      //Serial.println("Test");
      if (int s = receiveData(RX_Buffer_Len)) {
          #ifdef DEBUG_ANALYZER
          Serial.println(""); Serial.print(s); Serial.print("\t");
          #endif
        stateMachine = TRANSMIT;
        decodeServoData();
          #ifdef DEBUG_ANALYZER
          Serial.print(millis() - RX_last_frame_received); Serial.print("ms\t");
          #endif
        RX_last_frame_received = millis();
        
        RX_hopping_time = micros()  + (TX_period * 2);
        if (RX_RSSI < power_thr_low && power_delay_counter-- == 0) {
          power_increase();
          power_delay_counter = TX_POWER_DELAY_FILTER;
        }
        if (RX_RSSI > power_thr_high && power_delay_counter-- == 0) {
          power_decrease();
          power_delay_counter = TX_POWER_DELAY_FILTER;
        }
        failsafe_state = false;
      }
      break;
    case TRANSMIT:
      TX_Buffer[0] = RX_RSSI;
      TX_Buffer[1] = current_power;
      TX_Buffer[2] = 0;
      TX_Buffer[3] = 0;
      TX_Buffer[4] = 0;
      TX_Buffer[5] = 0;
      packet_timer = micros();
      sendBufferData();
      Hopping();
      stateMachine = RECEIVE;

      calculated_rssi = calculate_rssi(RX_RSSI);
      //calculated_lost_frames_rssi= calculate_lost_frames_rssi(lost_frames);
      
      manage_servos();
      

      //Serial.println(micros() - packet_timer);
      #ifdef DEBUG_ANALYZER
      for (int i = 0; i < SERVO_CHANNELS; i++) {
        Serial.print("\t");
        Serial.print(Servo_Buffer[i]);
      }
      #endif
      break;
  }
  // forced hopping if no data received
  // TODO: Fast recover then slow recover
  if (micros() > RX_hopping_time) {

    RX_hopping_time = micros()  + (TX_period * 2);
      
    Hopping();
    
    #ifdef DEBUG_CH_FREQ
    Serial.print("No FR: ");
    Serial.print(millis() - RX_last_frame_received);
    Serial.print("ms\thop: ");
    Serial.println(current_channel);
    #endif
  }
  if (micros() > RX_frame_time) {
    // IBUS
    #ifdef IBUS_module
      #ifdef DEBUG_RX_OUTPUT
        // Show delay of each frame
        Serial.print("\t Delayed[us]=");
        Serial.println(micros() - RX_frame_time, DEC);
      #endif //DEBUG_RX_OUTPUT
    if (!failsafe_state)
      send_servo_frame();
    // Show calculated RSSI
    //Serial.print("\t");
    //Serial.println(calculate_rssi(RX_RSSI));
    RX_frame_time = micros() + ibus_frame_period;
    #endif // IBUS_module

    // MSP
    #ifdef MSP_module
    // TODO: every 5th frame send request for data and block 
    // the send_servo_frame() until data is received - do not wait too long (max 2 frames)
      if (!failsafe_state)
        send_servo_frame();
    #endif // MSP_module
    if (failsafe_state)
      Serial.println("Failsafe !!!");
  }
  // Failsafe
  if (millis() - RX_last_frame_received > FAILSAFE_DELAY_MS) {
    // IBUS, MSP - stop transmitting data
    failsafe_state = true;
    // PPM - send data out of range
    set_servos_failsafe();
  }
      
  // obsługa konfiguracji
  
#endif //RX_module
  
  wdt_reset();
}
