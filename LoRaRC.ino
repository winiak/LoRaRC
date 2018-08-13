/**
 * LoRaLRS project by Konrad Winiarski
 * 
 * Using LoRa library developed by Sandeep Mistry: https://github.com/sandeepmistry/arduino-LoRa
 * Special thanks to:
 * - Jacek Szostak
 */

#include <SPI.h>
#include <LoRa.h>
#include "config.h"
#include <avr/wdt.h>

  stateMachineDef stateMachine = TRANSMIT;
  unsigned long TX_period = F_rate_low;  // 7700 / 20000 / 40000  us
  unsigned long ibus_frame_period = 7000; //us
  unsigned long timer_start, timer_stop;
  unsigned long lost_frames = 0;
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
  LoRa.setPins(10, 9, 4);
  if (!LoRa.begin(base_frequency)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setTxPower(tx_power_low); 
  LoRa.setSpreadingFactor(spread_factor); // 6 to 12 - 6 requires IMPLICIT
  
  LoRa.setSignalBandwidth(BW_high); 
  TX_period = F_rate_high;
  
  LoRa.enableCrc();
  //LoRa.disableCrc();

  
}

void loop() {
  static long int packet_timer;
  static unsigned long transmit_time = 0;
  static unsigned long receive_time = 0;
  static unsigned long twenty_ms_time = 0;
  static unsigned long ibus_frame_time = 0;
  static uint8_t no_RX_ack = 0; 

  #ifdef TX_module
  switch (stateMachine) {
    case TRANSMIT:    // transmit data then switch to RECEIVE
      TX_Buffer_Len = buildServoData();
      packet_timer = micros();
      sendBufferData();
      
      stateMachine = RECEIVE;
      break;
    case RECEIVE:     // stay in RECEIVE and wait for data until next TX period
      no_RX_ack++;
      if (receiveData(6)) {
        no_RX_ack = 0;
        TX_RSSI = RX_Buffer[0] - 255;
        Serial.print("\tTransmitted RSSI: ");
        Serial.print(TX_RSSI);
        Serial.print("\tReceived RSSI: ");
        Serial.print(RX_RSSI);
        Serial.print("\tPacket exchange time: ");
        Serial.println(micros() - packet_timer);        
      }
      break;
    case SETUP:
      break;
    default: // controller
      break;
  }
  if (micros() > transmit_time) {
    transmit_time = micros() + TX_period;
    timer_start = micros();
    stateMachine = TRANSMIT;
  }
#endif // TX_module


#ifdef RX_module
  TX_Buffer_Len = 6;
  RX_Buffer_Len = 12;
  
  switch (stateMachine) {
    case RECEIVE:
      if (receiveData(RX_Buffer_Len)) {
        stateMachine = TRANSMIT;
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
      Serial.println(micros() - packet_timer);
      stateMachine = RECEIVE;
      break;
  }

  // kopiowanie danych z bufora RX do serw
  // obsługa failsafe
  // obsługa generowania sygnalu wyjsciowego
  // obsługa konfiguracji
  
#endif //RX_module
  
  wdt_reset();
}

int receiveData(char data_len) {
  int packetSize = LoRa.parsePacket(data_len);
  char i = 0;
  if (packetSize) {
    Serial.print("[");
    while (LoRa.available()) {
      RX_Buffer[i] = (char)LoRa.read();
      Serial.print(RX_Buffer[i]);
      i++;
    }
    Serial.print("] \t");
    // print RSSI of packet
    RX_RSSI = LoRa.packetRssi();
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

