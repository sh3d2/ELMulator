# ELMulator

## ODBII & ELM327 Emulation library

This is a fork of the [GTTurboEcu library by TheBigBadWolfClub](https://github.com/TheBigBadWolfClub/GTTurboEcu) which is no longer being maintained. 

I have updated the library with the following changes:
- included fixes submitted to the upstream fork but never merged
- updated the library to work with Bluetooth serial devices (like esp32)
- added additional functions to the emulator
- optimized the use of String class for lower memory requirements
- fixed a few bugs from earlier version
- refactoring many methods to simplify use of the library  
- made it easier to add your own new functions with sample/test data
- implemented a simple emulation mode that can be easily configured with just a few lines of code 
- added new example programs to demonstrate use of the library 

This library allows you to program a microcontroller device to emulate an ELM327 OBDII adapter connected to an ECU. The emulator can work with any of the huge number of ODBII apps that are compatible with the ELM327-standard adapters, like Torque or CarScanner. 

There are two main use cases for this library:

1. It is ideal for use in developing OBDII applications using the [ELMduino](https://github.com/PowerBroker2/ELMduino) library. You can build the example program, upload it to a $10 ESP32 device with builtin Bluetooth and use it for your testing and development, avoiding the need to be communicating with an actual ELM327 device plugged into a vehicle. (Doing app development sitting on a folding chair in the garage gets old pretty quickly...).

2. This library can be used to as the building block for creating your own ELM327 compatible device that can read values from various custom sensors and allow you to monitor
those sensors with your own software or one of the many OBDII client applcations that are compatible with the ELM327 specification. No connection to an actual OBDII port is needed. 

## Platforms

ELMulator is built and tested using the Arduino framework on a variety of ESP32 dev boards. The ESP32 platform is ideal as it has Bluetooth, Wifi and serial capability included and is very inexpensive. The library should work with any Arduino compatible device with the use of additional hardware like the [HC05 Bluetooth Module](https://components101.com/wireless/hc-05-bluetooth-module) to provide Bluetooth capability. 

## Using ELMulator

In order to take best advantage of this library, you should be familiar with two specifications:

- The [OBDII protocol](https://en.wikipedia.org/wiki/OBD-II_PIDs)

- The [ELM327 datasheet](https://www.elmelectronics.com/DSheets/ELM327DSH.pdf)

You need to understand what a PID is and how it is defined (mode, pid, number of bytes, conversion formulas ...) and how to send/receive PID info from the ELM327 device.

### Easily create an ELM327 emulator with just a few lines of code:

```
#include "ESP32_Simple_ELMulator.h"

/**
 * This example demonstrates a very simple implementation of ELMulator.
 * It handles all AT Commands, SUPPORTED_PIDS queries, etc. and rejectss unsupported requests.
 * All Mode 01 PID requests are provided a response of the correct data size with mock data values.
 * Typical use case is for testing custom applications to work with ELM327 devices. 
 */

const String deviceName = "ELMULATOR"; // Bluetooth device name to use (no pin)
ELMulator ELMulator;

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting ELMulator...");
    ELMulator.init(deviceName, true); // Initialize with support for all PIDs 0x00 to 0x65, providing mock data
    ELMulator.begin();                // ELMulator will run continuously, listening for and respondng to requests
}

void loop() {}
```

That's it. Complile and install the program on an inexpensive ESP32 dev board and you can be running in a couple of minutes. You can connect your app or device to the Bluetooth device "ELMULATOR" and query it as though it were an ELM327 adapter connected to a vehicle. Very handy for testing, without the need to install and configure a physical adapter attached to an actual vehicle. 

###Custom ELM327-compatible programs/devices

You can also use ELMulator to provide custom responses to specific queries or as the basis for your own device that reads physical sensors and provides sensor data via the OBDII / ELM327 protocols. This example code shows responding to PID requests with hardcoded values, custom sensor data and mock sensor values to demonstrate some of the possibilities for creating your ELM327 compatible application or device. 

```
#include "ESP32_Custom_ELMulator.h"

const String deviceName = "ELMULATOR"; // Bluetooth device name to use (no pin)

// Set up hardcoded responses to non-standard PID requests for MIL, DTC, ODO data
const String milResponse = "4101830000";                       // MIL response code indicating 3 current DTC
const String dtcResponse = "43010341\r\n43010123\r\n43010420"; // DTC response returning 3 DTC codes (multiline response)
const uint32_t odoResponse = 1234567;                          // Hardcode an odometer reading of 1234567

ELMulator ELMulator;

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting ELMulator...");
    ELMulator.init(deviceName); 

    // Register the specific PIDs we are going to handle.
    // ELMulator will respond to SUPPORTED_PIDS request ("0101") with the appropriate value
    // indicating which PIDs it can respond to.
    ELMulator.registerMode01Pid(ODOMETER);
    ELMulator.registerMode01Pid(ENGINE_COOLANT_TEMP);
    ELMulator.registerMode01Pid(ENGINE_RPM);
    ELMulator.registerMode01Pid(MONITOR_STATUS_SINCE_DTC_CLEARED);
    ELMulator.registerMode01Pid(INTAKE_MANIFOLD_ABS_PRESSURE);
    ELMulator.registerMode01Pid(ENGINE_LOAD);
    ELMulator.registerMode01Pid(VEHICLE_SPEED);
    ELMulator.registerMode01Pid(INTAKE_AIR_TEMP);
    ELMulator.registerMode01Pid(THROTTLE_POSITION);
}

void loop()
{
    /**
     * Waits for a new request from an ELM client and does some pre-processing to validate the PID and request.
     * Non-PID requests like AT commands and SUPPORTED_PIDS queries will be handled by ELMulator automatically.
     * PID requests will be verified as supported (registered); unsupported PIDs responded to with NO DATA response.
     * Returns true if the request is for one of the PIDS we have registered above so we can respond to it.
     */
    if (ELMulator.readELMRequest())
    {
        handlePIDRequest(ELMulator.elmRequest);
    }
}

/**
 * After receiving a supported PID(sensor) request, we prepare the response here.
 * Response can come from a hardcoded value, a real hardware sensor, or a mock value.
 * Default response is a mock value if no other handling is provided.
 */
void handlePIDRequest(const String &request)
{
    // Handle special case requests like MIL and DTC checks
    if (ELMulator.isMode03(request)) // Mode 03 request == Returns (hardcoded) list current DTC codes
    {
        if (dtcResponse.length())
        {
            ELMulator.writeResponse(dtcResponse);
            return;
        }
        else
        {
            DEBUG("DTC response is empty.");
            ELMulator.writePidNotSupported();
            return;
        }
    }

    else if (ELMulator.isMode01MIL(request)) // Mode 0101 MIL request == Returns (hardcoded) number of current DTC codes
    {
        if (milResponse.length())
        {
            ELMulator.writeResponse(milResponse);
            return;
        }
        else
        {
            DEBUG("MIL response is empty.");
            ELMulator.writePidNotSupported();
            return;
        }
    }

    else // Handle any other supported PID request
    {
        uint8_t pidCode = ELMulator.getPidCode(request); // Extract the specific PID code from the request

        // Example response for 0x05 (Engine Coolant Temp) - returning a mock data value
        if (pidCode == ENGINE_COOLANT_TEMP)
        {
            uint32_t sensorValue = ELMulator.getMockSensorValue();

            /**
             * Response parameters:
             * request - specifies the original PID request being responded to.
             * numberOfBytes - the number of bytes this PID value returns (per OBDII spec), using lookup table responseBytes.
             * sensorValue - the value to return in our response; a mock value in this example.
             */
            ELMulator.writePidResponse(request, responseBytes[pidCode], sensorValue);
            return;
        }

        // Example response for PID 0x0C (Engine RPM) - returns a modified mock sensor value
        else if (pidCode == ENGINE_RPM)
        {
            uint16_t rpmValue = ELMulator.getMockSensorValue() * 100; // Here we multiply the mock value provided, for a more realistic RPM number
            ELMulator.writePidResponse(request, responseBytes[pidCode], rpmValue);
            return;
        }

        // Example response for PID 0xA6 (Odometer) - returns our hardcoded odometer response string
        else if (pidCode == ODOMETER)
        {
            ELMulator.writePidResponse(request, responseBytes[pidCode], odoResponse);
            return;
        }

        // // Example response for PID 0xOD (Vehicle speed) - returns a value from external GPS
        // else if (pidCode == VEHICLE_SPEED)
        // {
        //     uint32_t gpsSpeed = myGPS.speed();
        //     ELMulator.writePidResponse(request, responseBytes[pidCode], gpsSpeed);
        //     return;
        // }

        // Default response for any other supported PID request - returns an unmodified mock sensor value
        else
        {
            ELMulator.writePidResponse(request, responseBytes[pidCode], ELMulator.getMockSensorValue());
            return;
        }
    }
}

```
## Using ELMulator with an addon Bluetooth module (GPIO)

By default, ELMulator will work on an EPS32 with builtin Bluetooth, but you can configure another device type for use with a bluetooth module like the [HC05 Bluetooth Module](https://components101.com/wireless/hc-05-bluetooth-module)connected via GPIO and using SoftwareSerial.h library. To do so, the following two changes need to be made:

1. Update definitions.h file and undefine or set to false "BLUETOOTH_BUILTIN":

```
// true == We are running on hardware that has builtin Bluetooth
// false == We are using hardware that has BT module attached via GPIO pins
#define BLUETOOTH_BUILTIN true  
```
2. Provide the serial baud rate, Rx an Tx pin numbers when creating your ELMulator instance:

```
/**
* When creating a new instance of ELMulator with an attached
* BT module, we need to set the Bluetooth serial parameters
*
* @param baudRate - rate for bluetooth serial port (normally 9600)
* @param rxPin - the Arduino RX pin for the Bluetooth module (serial port)
* @param txPin - the Arduino TX pin for the Bluetooth module (serial port)
* @return
*/
ELMulator(uint32_t baudRate, uint8_t rxPin, uint8_t txPin);
```

Other than that, use of the ELMulator library is the same as when using builtin Bluetooth.

## License

The MIT License (MIT)

Copyright (c) 2023 Jim Whitelaw

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.