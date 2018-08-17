/**
 * LoRaLRS project by Konrad Winiarski
 * 
 * Using LoRa library developed by Sandeep Mistry: https://github.com/sandeepmistry/arduino-LoRa
 * Special thanks to Jacek S. for inspiration ;)
 * 
 */

#include <SPI.h>
#include <LoRa.h>
#include "config.h"
#include <avr/wdt.h>

  stateMachineDef stateMachine = TRANSMIT;
  unsigned long TX_period = F_rate_low;  // 7700 / 20000 / 40000  us
  unsigned long RX_last_frame_received, RX_hopping_time;
  unsigned long ibus_frame_period = 7000; //us
  unsigned long timer_start, timer_stop;
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
int counter = 0;

void setup() {
  Serial.begin(115200);
  // while (!Serial);
  wdt_enable(WDTO_250MS);
  
  Serial.println("LoRa Sender");
/*
  LoRa.setSignalBandwidth(125E3); // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, and 250E3.
  LoRa.setSpreadingFactor(7); // 6 to 12 - 6 requires IMPLICIT
  LoRa.setCodingRate4(5);   // 4 to 8, default 5
  LoRa.enableCrc(); // LoRa.disableCrc();
  LoRa.setTxPower(5); // 2 to 20, default 17

*/
  LoRa.setPins(10, 9, 2);   //(ss, reset, dio0) dio is optional but must be interrupt capable via attachInterrupt(...): prefered: nSS-10, nRESET-9, DI0-2
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
    Serial.println("TX RSSI\tRX RSSI\tLost\tPacket t[ms]\tPower [dBm]");
    #endif //TX_module
  #endif // DEBUG_ANALYZER
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
      servoTester();                    // servo tester - change servo values continuously
      Serial.println();
      Hopping();
      if (no_RX_ack > 0) {                // ACKonwlege, check for return of the telemetry data
        #ifdef DEBUG_RADIO_EXCH
        Serial.print("\t\t\t NO ACK: ");
        Serial.print(no_RX_ack);
        #endif
        if (no_RX_ack > TX_POWER_DELAY_FILTER) {             // if no ACK for (TX_POWER_DELAY_FILTER - two times hopping) frames, then assume no telemetry (and RSSI) is available
          TX_RSSI = 0;
          RX_RSSI = 0;
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
        TX_RSSI = RX_Buffer[0];// - 255;  to display byte values instead of dBm
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
      Serial.print(TX_RSSI);
      Serial.print("\t");
      Serial.print(RX_RSSI);
      Serial.print("\t");
      Serial.print(no_RX_ack);
      Serial.print("\t");
      Serial.print(byte((micros() - packet_timer)/1000));
      Serial.print("\t");
      Serial.print(current_power);
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
      if (int s = receiveData(RX_Buffer_Len)) {
        Serial.println(""); Serial.print(s); Serial.print("\t");
        stateMachine = TRANSMIT;
        decodeServoData();
        Serial.print(millis() - RX_last_frame_received); Serial.print("ms\t");
        RX_last_frame_received = millis();
        RX_hopping_time = micros()  + (TX_period * 2);
      }
      break;
    case TRANSMIT:
      TX_Buffer[0] = RX_RSSI;
      TX_Buffer[1] = 0;
      TX_Buffer[2] = 0;
      TX_Buffer[3] = 0;
      TX_Buffer[4] = 0;
      TX_Buffer[5] = 0;
      packet_timer = micros();
      sendBufferData();
      Hopping();
      stateMachine = RECEIVE;

      //Serial.println(micros() - packet_timer);
      for (int i = 0; i < SERVO_CHANNELS; i++) {
        Serial.print("\t");
        Serial.print(Servo_Buffer[i]);
      }
      break;
  }
  // forced hopping (2 times slower than TX) if no data received
  if (micros() > RX_hopping_time) {
    RX_hopping_time = micros()  + (TX_period * 2);
    Hopping();
    Serial.print(" No frame since: "); Serial.print(millis() - RX_last_frame_received); Serial.print("ms\thop: "); Serial.println(current_channel); 
  }

  // kopiowanie danych z bufora RX do serw
  // obsługa failsafe
  // obsługa generowania sygnalu wyjsciowego
  // obsługa konfiguracji
  
#endif //RX_module
  
  wdt_reset();
}

byte power_increase() {
  if (current_power < tx_power_high)
    current_power = current_power + tx_power_step;
  else
    return current_power;
  LoRa.sleep();
  LoRa.setTxPower(current_power);
  LoRa.idle();
  return current_power;
}

byte power_decrease() {
  if (current_power > tx_power_low)
    current_power = current_power - tx_power_step;
  else
    return current_power;
  LoRa.sleep();
  LoRa.setTxPower(current_power);
  LoRa.idle();
  return current_power;
}

void Hopping() {
  long f;
  LoRa.sleep();
  current_channel++;
  if (current_channel >= sizeof(hop_list))
    current_channel = 0;
  f = base_frequency + (hop_list[current_channel] * frequency_step);  //
  #ifdef DEBUG_CH_FREQ
    Serial.print(f);
  #endif
  LoRa.setFrequency(f);
  LoRa.idle();
}

int receiveData(char data_len) {
  int packetSize = LoRa.parsePacket(data_len);
  char i = 0;
  if (packetSize) {
    //Serial.print("[");
    while (LoRa.available()) {
      RX_Buffer[i] = (char)LoRa.read();
      //Serial.print(RX_Buffer[i]);
      i++;
    }
    //Serial.print("] \t");
    // print RSSI of packet
    RX_RSSI = LoRa.packetRssi() + 255;  // remove +255 to display byte values instead of dBm
    //Serial.print("RX RSSI: ");
    //Serial.println(RX_RSSI);    
    //Serial.print(" SNR: ");
    //Serial.println(LoRa.packetSnr());
  }
  RX_Buffer_Len = packetSize;
  return packetSize;
}

bool sendBufferData() {
  char sent_data_len;
  if (TX_Buffer_Len == 0)
    return false;
  LoRa.beginPacket(true);   // false - explicit, true - implicit
  sent_data_len = LoRa.write(TX_Buffer, TX_Buffer_Len);
  LoRa.endPacket();
  TX_Buffer_Len = 0;
  return sent_data_len;
}

