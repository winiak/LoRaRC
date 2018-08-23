#ifdef TX_module
#ifdef PPM_module

static bool PPM_corrupted = true;
static char PPM_corrupted_counter = 0;
static char channel_no = 0;
static char channel_counter = 0;

void setup_module() {
  Config_ICP1_PPM();
  sei();
}

bool check_PPM_corrupted () {
  return PPM_corrupted;
}

/**
 * Configure Timer to capture PPM values
 */
void Config_ICP1_PPM()        // Use ICP1 in input capture mode
{  
   pinMode(PPM_IN, INPUT);
   
   // Setup timer1 for input capture (PSC=8 -> 0.5ms precision)
   TCCR1A = ((1 << WGM10) | (1 << WGM11));
   TCCR1B = ((1 << WGM12) | (1 << WGM13) | (1 << CS11) | (1 <<ICNC1));  //input canceler, 
   // normally capture on rising edge, allow invertting via SW flag
   TCCR1B |= (1 << ICES1);    //ICES1 for edge selection 
   OCR1A = 65535;
   TIMSK1 |= (1 << ICIE1);   // Enable timer1 input capture interrupt

}

ISR(TIMER1_CAPT_vect)
{
  unsigned int time_temp;
  unsigned int servo_temp;
  unsigned int servo_temp2;
  

  time_temp = TCNT1; // read the timer1 value
  TCNT1 = 0; // reset the timer1 value for next
                        
  if (time_temp > 6000) // new frame detection : >4ms LOW
    { 
      channel_counter = channel_no;
      channel_no = 0;
    }
    else  {
      // check the signal time and update the channel if it is between 750us-2500us
      if ((time_temp > 1500) && (time_temp < 5000)) {
          Servos[channel_no] =  time_temp / 2; // write the low byte of the value into the servo value buffer.
          if (channel_no < 8) channel_no++;
          PPM_corrupted_counter = 0;
          PPM_corrupted = false;
      } else {
        PPM_corrupted_counter++;
        if (PPM_corrupted_counter > 10) {
          PPM_corrupted =  true;
          channel_counter = 0;
        }
        
      }
    }
}

#endif // PPM_module
#endif // TX_module


/**
 * 
 * Receiver - PPM signal generation
 * 
 */
#ifdef RX_module
#ifdef PPM_module

void setup_module() {
  ;
}

#endif // PPM_module
#endif // TX_module

