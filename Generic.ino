

uint8_t calculate_lost_frames_rssi(unsigned long lost_frames) {
  // calculate RSSI basing on lost frames
  // lost frames may be given as
  // - a counter of lost frames - no filter - short response, first good frame resets to zero, difficult to see when frame losts are shorter than half a second
  // - an average of frames lost in portion of time - slight delay in response, easier to see when conditions are going bad
  return 100 - round(100 * (lost_frames > LOST_FRAMES_COUNT ? LOST_FRAMES_COUNT : lost_frames) / LOST_FRAMES_COUNT); // 30 frames is a history
}

/**
 * Calculate RSSI based RSSI reported by transmitter
 * Return in %.
 * Easy but not reliable - I reported significant packets losts (Failsafe activated) when signal dropped below 55% (-102dB) 
 * It looks like is would be good to adapt the dynamics to sensitivity (SF and BW dependent) and TX power
 */
uint8_t calculate_rssi(int tr_rssi) {
  // Simpliest method used from Lora-net project
  tr_rssi = 157 + tr_rssi; // entire link budget

  // Method based on OpenLRSng (although not identical)
  // if the result from the link budget is less than 50
  // use lost packets as the remaining  50% calculation
  if (tr_rssi < 50) {
    // TODO
  }
  // Setting frames to show it in "%"
  if (tr_rssi > 100) tr_rssi = 100;
  if (tr_rssi < 0)  tr_rssi = 0;
  return (failsafe_state ? 0 : tr_rssi);
}

void manage_servos() {
  if (failsafe_state)
      set_servos_failsafe();
    else
      for (uint8_t i = 0; i < SERVO_CHANNELS; i++)
        Servos[i] = Servo_Buffer[i];
  #ifdef INJECT_RSSI_IN_CH
    Servos[INJECT_RSSI_IN_CH] = map(calculated_rssi, 0, 100, 1000, 2000);
    //Servos[INJECT_RSSI_IN_CH+1] = map(calculated_lost_frames_rssi, 0, 100, 1000, 2000);
    //Servos[INJECT_RSSI_IN_CH+2] = map(current_power, tx_power_low, tx_power_high, 1000, 2000);
  #endif
}

void set_servos_failsafe() {
  for (uint8_t i = 0; i < SERVO_CHANNELS; i++)
        Servos[i] = Servo_Failsafe[i];
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
    RX_RSSI = LoRa.packetRssi();  // remove +255 to display byte values instead of dBm
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

void PrintHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
   Serial.print("0x");
   for (int i=0; i<length; i++) {
     if (data[i]<0x10) {Serial.print("0");}
     Serial.print(data[i],HEX);
     Serial.print(" ");
   }
}
