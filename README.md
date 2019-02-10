# arduino-projects
Assorted code segments for Arduino processor boards. This repository will be updated as I create more code segments.

## generic_cmdline
Compatibility: All Arduino boards and clones with a serial interface available.

Purpose: Provides a sample serial terminal for accepting and responding to commands.

## hotelcard
Compatibility: All Arduino boards and clones with a serial interface available.

Purpose: Used to add hotel card verification to a vending machine. Cuts power to Nayax VPOS device and MDB cash devices. Enabled by a card swipe or open door, and disabled by something triggering the vend sensor or a timer expiring.

## st7920_serial_lcd
Compatibility: Arduino Mega 2560. Confirmed to NOT work on Arduino Pro Mini. Probably a memory usage issue.

Purpose: Provides a serial interface to the drawing commands for a ST7920 display using the U8G2 library from https://github.com/olikraus/u8g2

Written for a 128x64 ST7920 LCD, such as the XC4617 from Jaycar (https://www.jaycar.com.au/arduino-compatible-128x64-dot-matrix-lcd-display-module/p/XC4617) using the parallel interface. 115200,8,N,1 serial interface.
