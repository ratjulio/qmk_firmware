ADB to USB Keyboard Converter for RP2040
This is a port of the ADB to USB keyboard converter firmware for the RP2040 microcontroller, such as the Raspberry Pi Pico.

Hardware Requirements
RP2040 board (Raspberry Pi Pico recommended)
ADB keyboard
1kΩ resistor for the ADB data line
Basic wiring supplies
Wiring
Connect your RP2040 to the ADB keyboard as follows:

ADB Data (Pin 4) → GP0 on the RP2040 with a 1kΩ pull-up resistor to 5V
ADB Power (Pin 2) → 5V
ADB Ground (Pin 3) → GND
Note: The RP2040 operates at 3.3V, but the pins are 5V tolerant, which makes them compatible with the ADB protocol.

Installation
Set up your QMK environment
Clone the QMK firmware repo: git clone https://github.com/qmk/qmk_firmware.git
Create the directory keyboards/converter/adb_usb_rp2040
Copy these files into that directory
Compile and flash to your RP2040 board:
qmk compile -kb converter/adb_usb_rp2040 -km default
Pinout Configuration
ADB Data Pin: GP0 (can be changed in config.h)
ADB Power Control: GP1 (optional, can be changed in config.h)
Supported ADB Keyboard Features
All standard keyboard keys
Caps Lock, Num Lock, and Scroll Lock LEDs
Extended keyboard features where applicable
Troubleshooting
If the keyboard isn't recognized, check your connections and make sure you've included the pull-up resistor
Verify that your ADB keyboard is working with other ADB devices
Some ADB keyboards may need a reset sequence (hold Command+Option while plugging in)
Credits
This port is based on the original AVR implementation by TMK in the QMK firmware.

