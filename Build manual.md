#Manual

Build your own module.

To build your first LoRaRC module you need:

- Arduino Pro Mini 16Mhz, 5V (unless you can buy 16MHz 3,3V),
- AMS1117 3,3V voltage regulator,
- RFM95 module,
- 868MHz dipole antenna.

#Step 1 - Upload Optiboot bootloader into Arduino's Atmega microcontroller.
You can use various methodos to do it. The instruction is described in the
Optiboot Project's site: https://github.com/Optiboot/optiboot/wiki/InstallingOnChips

If you have any spare Arduino board, the easiest way is to use Optiloader
sketch which will upload the right bootloader and set the fusebit for you.
See Optiloader site: https://github.com/WestfW/OptiLoader.

All you need to do is to connect the boards as follows and power on the "programmer board":
Programmer board	LoRaRC board
	Pin 13				Pin 13
	Pin 12				Pin 12
	Pin 11				Pin 11
	Pin 10				Pin RESET
	+5V VCC				+5V VCC / RAW *
	GND					GND

* IMPORTANT NOTE: If you already have soldered Arduino and RFM togeather you cannot 
connect +5V to LoRaRC module VCC. RFM is not 5V tolerant. Make sure you will supply
the module with 3.3V. See points below.

Finally, try to upload any sketch to your Arduino Pro Mini. Remember, from now on
your Pro Mini's bootloader will communicate with baudrate of 115200bps. Therefore,
select Board Type as: Arduino/Genuino Uno.

#Step 2 - Change Arduino's voltage regulator to 3.3V
Since RFM95 is not 5V tolerant, but Atmega is 3.3V tolerant, the easiest way is to 
change Arduino's VCC to 3.3V. In order to achieve that, we need to change voltage
regulator. 

First, desolder original 5V regulator.

AMS1117 spec sheet: http://www.advanced-monolithic.com/pdf/ds1117.pdf

#Step 3 - Solder RFM95 and Arduino togeather
http://www.hoperf.com/upload/rf/RFM95_96_97_98W.pdf

Kynar

Wiring:
RFM95		Arduino
MISO	<->	MISO (Pin 12)
MOSI	<->	MOSI (Pin 11)
SCK		<->	SCK (Pin 13)
NSS		<->	Pin 10
NRESET	<->	Pin A1
DIO0	<->	Pin 2  
3.3V	<->	VCC (3.3V)
GND		<->	GND

#Step 4 - Build your own 868MHz antenna

