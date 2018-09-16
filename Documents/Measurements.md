See "DEBUG ANALYZER" output of TX module:

868600000       HC: 0   Tr RSSI: -46    Re RSSI: -55    Packet exchange time: 22036
868800000       HC: 1   Tr RSSI: -46    Re RSSI: -55    Packet exchange time: 21924
869300000       HC: 2   Tr RSSI: -46    Re RSSI: -56    Packet exchange time: 21932
868600000       HC: 0   Tr RSSI: -46    Re RSSI: -56    Packet exchange time: 21940
868800000       HC: 1   Tr RSSI: -46    Re RSSI: -55    Packet exchange time: 21932
869300000       HC: 2   Tr RSSI: -46    Re RSSI: -56    Packet exchange time: 22036
868600000       HC: 0   Tr RSSI: -46    Re RSSI: -55    Packet exchange time: 21944
868800000       HC: 1   Tr RSSI: -47    Re RSSI: -56    Packet exchange time: 22036
869300000       HC: 2   Tr RSSI: -46    Re RSSI: -56    Packet exchange time: 21912
868600000       HC: 0   Tr RSSI: -46    Re RSSI: -56    Packet exchange time: 22036
868800000       HC: 1   Tr RSSI: -46    Re RSSI: -56    Packet exchange time: 22036
869300000       HC: 2   Tr RSSI: -47    Re RSSI: -56    Packet exchange time: 22048

First column is frequency, second Hopping Channel.
Thrid column is the RSSI in RX module (strngth of signal comming from Transmitter), send back to transmitter in telemetry channel.
Forth column is RSSI in TX module (strength of signal comming from Receiver).
Fith colum is packet exchange time - total time from sending TX data to RF chip (via SPI) 
until receiving the telemetry back from RX module. Current values are at SF=6 and BW=250k.


"DEBUG ANALYZER" and "SERVO DATA" output of RX module (no serial-based module like IBUS or SBUS selected):

12b	22ms	4dBm		2000	2000	1000	2000	2000	2000	2000	2000
12b	23ms	4dBm		1980	1980	1000	1980	1980	1980	1980	1980
12b	22ms	4dBm		1960	1960	1000	1960	1960	1960	1960	1960
12b	24ms	4dBm		1940	1940	1000	1940	1940	1940	1940	1940
12b	22ms	4dBm		1920	1920	1000	1920	1920	1920	1920	1920
12b	23ms	4dBm		1900	1900	1000	1900	1900	1900	1900	1900
12b	23ms	4dBm		1880	1880	1000	1880	1880	1880	1880	1880
12b	23ms	4dBm		1860	1860	1000	1860	1860	1860	1860	1860
12b	22ms	4dBm		1840	1840	1000	1840	1840	1840	1840	1840
12b	23ms	4dBm		1820	1820	1000	1820	1820	1820	1820	1820
12b	23ms	4dBm		1800	1800	1000	1800	1800	1800	1800	1800
12b	22ms	4dBm		1780	1780	1000	1780	1780	1780	1780	1780
12b	23ms	4dBm		1760	1760	1000	1760	1760	1760	1760	1760
12b	23ms	4dBm		1740	1740	1000	1740	1740	1740	1740	1740
12b	23ms	4dBm		1720	1720	1000	1720	1720	1720	1720	1720
12b	22ms	4dBm		1700	1700	1000	1700	1700	1700	1700	1700

First colum is a number of bytes recieved (always 12 anyway).
Second column is total time between frames, including generating servo signals and sending them to serial.
Third column is current transmitting power in dBm (at the moment the range is between 4 to 10dBm).
Followig columns are servo channels. These values are the result of "TX_SERVO_TESTER" turned on (continuous swipe of the signal, except Throttle channel, just in case...).


Some findings during testing:

<b>Testing Asynchronous mode</b>
Due to asynchronous mode, there is bigger variation of packet(frame) length. 
Although it is now possible to shorten entire frame time (from 25 down to 22..23ms). 
Unfortunately I observed single frames losses more often (too long wait time 
for the next frame). In order to improve recovery, I added two possible hop-and-wait-for-packet delay time values.

