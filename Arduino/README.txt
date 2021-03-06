Wiring Lite/Arduino
v0.3.7 - 23 May 2005
http://ardiuno.berlios.de/
Massimo Banzi, David Cuartielles, David A. Mellis, Nick Zambetti

For Wiring, by Hernando Barragan
Based on Processing, by Ben Fry and Casey Reas
Includes avrlib by Pascal Stang and some of unxutils

Wiring Lite lets you use the Wiring IDE and (parts of) the API
with the Arduino board (based on the Atmega8).  First, you need
to install the bootloader using an in-system-programmer, then you
can program the board over the serial port.  Or, on Windows, you
can build your own in-system-programmer for the paralle port.

Current commands include: digitalRead, digitalWrite, analogRead,
analogWrite (on digital pins 9 and 10 only), delay, millis,
beginSerial, serialWrite, and maybe print.

For a list of current bugs, see the online database at:
https://developer.berlios.de/bugs/?group_id=3590

WARNING: install Wiring Lite to a directory with no spaces in its name!

NOTE: on Windows, if you get an error of that sounds something like
"'bin': no such command, or executable found" or "'..': no such command
or executable found", try running the install-w2k.bat instead of the
install.bat.  Caused by funkiness with forward and backward slashes.

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
   Windows XP: edit install.bat to specify the directory to Wiring
   (make sure it has no spaces in it).  Then double-click install.bat.
   Windows 2000: edit install-w2k.bat to specify the directory to Wiring
   (make sure it has no spaces in it).  Then double-click install-w2k.bat.
3. Windows: If you're using a parallel port programmer, you need to
   install giveio: unzip giveio.zip and run ginstall.bat.

USAGE INSTRUCTIONS
1. Run Wiring 0003.
2. Write, compile, and export your code as normal; it will be
   processed for Wiring Lite/Arduino instead of Wiring.

UNINSTALLATION INSTRUCTIONS
1. Rename or move the <WIRING>/lib/wiringlite directory.
(Put it back to reinstall.)

CHANGELOG

v0.3.6
Added a timeout to analogRead, as it was sometimes going into an infinite loop.
Added alternative upload options to the makefile (for an ISP or parallel programmer).
Fixed Windows path problems which required having rm, cp, etc. in path (e.g. having WinAVR)
Fixed digitalWrite and digitalRead on pins 9 and 10.
Added Windows 2000 specific makefiles and installer to fix problem with forward slashes.
Made Mac OS X installer flexible enough to handle names for Wiring.app.
Switched the default Windows uploader to avrdude (with a slightly modified .conf file)
Added avrdude to the distribution.
Added giveio.zip to the distribution (for use with parallel port programmers).

v0.3.5 Fixed analogRead (using avrlib's a2d)
v0.3.4 Now using the serial port specified by Wiring.  Minor bug fixes.
v0.3.2 Split the wiringlite/include/makefile into mac and windows versions.
