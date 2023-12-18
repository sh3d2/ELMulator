# ELMulator
ODBII & ELM327 Emulation library

This is a fork of the [GTTurboEcu library by TheBigBadWolfClub](https://github.com/TheBigBadWolfClub/GTTurboEcu) which is no longer being maintained. 

I have updated the library with the following changes:
- included fixes submitted to the upstream fork but never merged
- updated the library to work with Bluetooth serial devices (like esp32)
- added additional functions to the emulator 
- made it easier to add your own new functions with sample/test data
- implemented a pseudorandom test data generator function 
- added a new example program to demonstrate use of the library 

This library allows you to program a microcontroller device to emulate an ELM327 OBDII adapter connected to an ECU. The emulator can work with any of the huge number of ODBII apps that are compatible with the ELM327-standard adapters, like Torque or CarScanner. 

There are two main use cases for this library:

1. It is ideal for use in developing OBDII applications using the ELMduino library. You can build the example program, upload it to a $10 ESP32 device (Bluetooth built in) and use it for your testing and development, avoiding the need to be communicating with an actual ELM327 device plugged into a vehicle. 

2. This library can be used to as the building block for creating your own ELM327 compatible device that can read values from various custom sensors and allow you to monitor
those sensors with your own software or one of the many OBDII client applcations that are compatible with the ELM327 specification. No connection to an actual OBDII port is needed. 

ELMulator is built and tested using the Arduino framework on a variety of ESP32 dev boards. The ESP32 platform is ideal as it has Bluetooth, Wifi and serial capability included and is very inexpensive. The library should work with any Arduino compatible device with the use of additional hardware like the [HC05 Bluetooth Module](https://components101.com/wireless/hc-05-bluetooth-module) to provide Bluetooth capability. 


In order to take best advantage of this library, you should be familiar with two specifications:

- The [OBDII protocol](https://en.wikipedia.org/wiki/OBD-II_PIDs)

- The [ELM327 datasheet](https://www.elmelectronics.com/DSheets/ELM327DSH.pdf)

You need to understand what a PID is and how it is defined (mode, pid, number of bytes, conversion formulas ...) and how to send/receive PID info from the ELM327 device.  

# License

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Copyright (c) 2017 . All right reserved.
