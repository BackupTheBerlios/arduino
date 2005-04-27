Wiring Lite/Arduino v0.3
Massimo Banzi, David Cuartielles, David A. Mellis, Nick Zambetti
For Wiring, by Hernando Barragan
Based on Processing, by Ben Fry and Casey Reas
Includes avrlib by Pascal Stang and some of unxutils
21 April 2005

Wiring Lite lets you use the Wiring IDE and (parts of) the API
with the Arduino board (based on the Atmega8).  First, you need
to install the bootloader using an in-system-programmer, then you
can program the board over the serial port.

Current commands include: digitalRead, digitalWrite,
analogWrite (on digital pins 9 and 10 only), delay, millis,
beginSerial, serialWrite, print.  Serial communication, however,
seems to only work on the PC.

BOOTLOADER INSTRUCTIONS
1. Make sure that uisp (in <WIRING>/tools/avr/bin) is in your path.
2. Plug an in-system-programmer into your Arduino board and computer.
3. Plug the Arduino into a power supply
4. Making sure that the serial device, specified with -dserial in
   the script, is correct, double-click the appropriate script
   Mac: bootloader/program.command
   Windows: bootloader/program.bat

INSTALLATION INSTRUCTIONS
1. Install Wiring 0003.
2. Mac: double-click install.command
   Windows: double-click install.bat
3. Enter the path to Wiring when prompted

USAGE INSTRUCTIONS
1. Run Wiring 0003.
2. Write, compile, and export your code as normal; it will be
   processed for Wiring Lite/Arduino instead of Wiring.

UNINSTALLATION INSTRUCTIONS
1. Rename or move the <WIRING>/lib/wiringlite directory.
2. (Put it back to reinstall.)

CHANGELOG
v0.3.1 Split the wiringlite/include/makefile into mac and windows versions.