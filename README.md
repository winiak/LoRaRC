# LoRaRC
LoRa for Remote Control

The goal of this project is to use LoRa modules (RFM95/96) with Arduino to remotely control an aircraft or drone. The expectation is that using LoRa modules shall improve flight distance due to great sensitivity. The drawback is data rate limited by the technology.

Beware!
This project is for hobby and technology "exploration" purposes. Not for commercial use! You are free to use it for your personal purposes. I am not taking the responsibility for the robustness or the stability of this solution. Use at your own risk. Make sure you are working within your country regulations and limitations. 
See frequencies over the countries: https://www.thethingsnetwork.org/docs/lorawan/frequency-plans.html
Play safe, have fun :).

There are following targets to be achieved in this project ("+" means DONE):
+ (+)Low cost communication modules (RFM95 + Arduino ProMini)
- ready to use Arduino_Lora library, https://github.com/sandeepmistry/arduino-LoRa - requires updates or will be included as a part of this project
+ (+)TX and RX functionality in the same code - to simplify configuration, just select it by #define in config.h
+ (+)Bi-directional communication, possibility to read (limited) telemetry
+ (+)Frequency hopping, possibility to configure number of hop channels and frequencies
+ (+)Dynamic TX Power adjustment, based on RSSI level, configurable power range
- Dynamic Lora Bandwidht, depending most likely on lost frames
+ (+)Maximum 8 channels (compressed with method known from SBUS) to send 12 bytes
+ (+)Response frame limited to 6 bytes
- Possible frame rates: 12ms, 25ms, 45ms, 85ms (SF6, CRC, BW500..250..125..62.5KHz). Can be 30% less if telemetry will be turned off, but then fixed Power and fixed Bandwith must be applied - not in plans at the moment.
+ (+)RSSI is provided and injected to channel 8 (can be configured) and it is based on RSSI signal of LoRa modules.
- RSSI based on lost frames (TODO)
- External protocols: 
  + (+)PPM as input
  + (+)IBUS (FlySky protocol - 7ms update time, 115kbs, non-inverted serial, can be used with native Serial of Arduino) as output
  - IBUS as input (TODO)
  + (+)SBUS as output (DONE)
  - SBUS as input (TODO)
  - protocol for telemetry: MSP (TODO)
  - TBS CRLF as  input (some day..., anyone???)
- telemetry data will be limited to several parameters, send in compressed form, with one parameter at a time (or few but limited to 4..5 bytes):
  - Signal strength, Voltage, Current, Distance, 
  - Altitude, Angle Pitch, Angle Roll, Home direction
  - GPS Long
  - GPS Lat
  (that should be all what is mandatory when you loose Video link and may be needed to find lost plane)
+ (+)Failsafe in receiver to be provided (PPM data out of range, IBUS stops sending data)
- Setup process is considered (to configure and store data without need of reprogramming)
- Binding process is considered, although, currently there is no header in the data frame, only CRC is checked, therefore frequency hopping protects us from failure ;)

Hardware:
- Arduino Pro Mini 5V/16MHz with voltage regulator changed to 3,3V
- RFM95 for 868MHz (see https://github.com/sandeepmistry/arduino-LoRa for wiring description)
- Antenna (Dipole, Moxon)

Testing:
- First in flight testing (not as controller) - altitude up to 800m and distance up to 900m. Video taken during the flight: https://youtu.be/bZgweGUDXOE ; Report from the flight: https://github.com/winiak/LoRaRC/blob/master/Reports/link_test_19.08.2018.xlsx
- Live testing in flight... I did already ~20 flights (status on 12.09.2018), some fixes were done (failsafe issue, power management thresholds). 
- The longest flight was 5,8km at altitude of 300m (another one at 200m) where the video link was a limit. Power set in both TX and RX was set to 10mW! See video, log can be provided if needed: https://www.youtube.com/watch?v=st8SkwsevAg&index=4&list=PLqifu3lTwTqqz3f-RLDulBpYAeGYek0TZ



https://revspace.nl/DecodingLora
https://www.thethingsnetwork.org/docs/lorawan
http://www.hoperf.com/upload/rf/RFM95_96_97_98W.pdf
http://www.semtech.com/apps/filedown/down.php?file=SX1272LoRaCalculatorSetup1%271.zip
