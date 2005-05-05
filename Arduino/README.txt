Wiring Lite/Arduino v0.3.5
Massimo Banzi, David Cuartielles, David A. Mellis, Nick Zambetti
http://ardiuno.berlios.de/
5 May 2005

For Wiring, by Hernando Barragan
Based on Processing, by Ben Fry and Casey Reas
Includes avrlib by Pascal Stang and some of unxutils

Wiring Lite lets you use the Wiring IDE and (parts of) the API
with the Arduino board (based on the Atmega8).  First, you need
to install the bootloader using an in-system-programmer, then you
can program the board over the serial port.

Current commands include: digitalRead, digitalWrite, analogRead,
analogWrite (on digital pins 9 and 10 only), delay, millis,
beginSerial, serialWrite, and maybe print.

WARNING: install Wiring Lite to a directory with no spaces in its name!

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
2. Mac: double-click install.command, enter the path to Wiring
   when prompted (make sure it has no spaces in it).
   Windows: edit install.bat to specify the directory to Wiring
   (make sure it has no spaces in it).  Then double-click install.bat.

USAGE INSTRUCTIONS
1. Run Wiring 0003.
2. Write, compile, and export your code as normal; it will be
   processed for Wiring Lite/Arduino instead of Wiring.

UNINSTALLATION INSTRUCTIONS
1. Rename or move the <WIRING>/lib/wiringlite directory.
2. (Put it back to reinstall.)

CHANGELOG
v0.3.2 Split the wiringlite/include/makefile into mac and windows versions.
v0.3.4 Now using the serial port specified by Wiring.  Minor bug fixes.
v0.3.5 Fixed analogRead (using avrlib's a2d)