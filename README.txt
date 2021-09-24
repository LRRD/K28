upload_K28_smt_contained is used to setup K28 SMT v2 9/2021

AVRDUDE version 5.10 download: http://download.savannah.gnu.org/releases/avrdude/
USBtinyISP driver download: https://learn.adafruit.com/usbtinyisp/drivers

The batch file runs series of commands using command prompt in windows
The whole folder titled K28_code_v2 needs to be copied to the local PC from the network drive to function properly

If the code is changed, a new hex file needs to be generated using the Arduino IDE
Sketch -> Export Compiled Binary



PRE-SETUP:

- Connect K28 using the USBtiny ICSP programming cable (This contacts 6 pins needed for software changes: 5V, GND, SCK, MISO, 	MOSI, RESET
- Connect USBtiny to the PC
- Open device manager in windows and you should see libusb-win32 devices
	This verifies the USBtiny programmer is detected and the drivers are working correctly




SETUP USING BATCH FILE (RECOMMENDED):

- Copy the folder titled "K28_Code_v2" to your desktop from the network drive
- Open the folder and run "upload_K28_smt_contained"
- Press space or any other key to walk through automated setup process