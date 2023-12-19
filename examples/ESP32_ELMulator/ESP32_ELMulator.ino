#include "ESP32_ELMulator.h"

String deviceName = "ELMULATOR";
bool isCycleUp = true;
uint32_t cycle = 0;

String milResponse = "4101830000";                       // MIL response code indicating 3 current DTC
String dtcResponse = "43010341\r\n43010123\r\n43010420"; // DTC response returning 3 DTC codes (multiline response)
uint32_t odoResponse = 1234567;                          // Hardcode an odometer reading of 1234567

ELMulator ELMulator;

void setup()
{
    setupSerial();
    ELMulator.init(deviceName);

    // Set up PIDs 0x00 to 0x65
    // Will be provided a response with mock data value if no special handling is performed.
    // Requests for PID > 0x65 are not currently supported - will return "NO DATA"
    for (int i = 0; i < 0x66; i++)
    {
        ELMulator.registerMode01Pid(i);
    }
}

void loop()
{
    String pidRequest = ELMulator.readPidRequest();
    handleRequest(pidRequest);
}

void setupSerial()
{ // init serial communication, is only need if there's a need
    //  to read the debug info on serial port of the arduino

    Serial.begin(115200);
    Serial.setHwFlowCtrlMode(HW_FLOWCTRL_DISABLE);
    Serial.println("ELMulator starting up . . .");
}

/**
 * Fake some data
 * start counter = 0
 * every call to getFakeSensorData, will increment the counter to a max of 255 (0xff)
 * When 0xff is reached the cycle is reverted
 * and the counter will decrement by one until 0 is reached
 */
uint32_t FakeSensorValueProvider()
{

    if (isCycleUp)
    {
        cycle++;
        if (cycle == 0xff)
        {
            isCycleUp = false;
        }
    }
    else
    {
        cycle--;
        if (cycle == 0)
        {
            isCycleUp = true;
        }
    }

    return cycle;
}

bool isMode01(String command) {
    return command.startsWith("01") ? true : false;
}

bool isMode03(String command)
{
    return command.startsWith("03") ? true : false;
}

bool isMode01MIL(String command)
{
    return command.startsWith("0101") ? true : false;
}

void handleRequest(String pidRequest)
{
    /**
     * After receiving a PID(sensor) request
     * read the sensor value and send a response.
     * Response can come from a sensor or mock value.
     */

    // Handle special case requests like MIL and DTC checks

    if (isMode03(pidRequest)) // Mode 03 request == get current DTC codes
    {
        if (dtcResponse.length())
        {
            ELMulator.writeResponse(dtcResponse);
            return;
        }
        else
        {
        }
    }

    else if (isMode01MIL(pidRequest)) // Mode 0100 MIL request gets number of current DTC codes
    {
        if (milResponse.length())
        {
            ELMulator.writeResponse(milResponse);
            return;
        }
        else
        {
            DEBUG("MIL response is empty.");
            return;
        }
    }
    
    else  if (isMode01(pidRequest)) // Check for mode 01 request 
    {
        if (pidRequest.length() > 4)
        {
            pidRequest = pidRequest.substring(0, 4); // remove num_responses value if present; lib only handles single responses
        }
        uint16_t hexCommand = strtoul(pidRequest.c_str(), NULL, HEX);
        uint8_t pidCode = ELMulator.getPidCodeOnly(hexCommand);

        // Example response to PIC 0xA6 (Odometer) - return hardcoded mock value
        if (pidCode == ODOMETER)
        {
            ELMulator.writePidResponse(pidRequest, responseBytes[pidCode], odoResponse);
            return;
        }

        //example processing for 0x05 (Engine Coolant Temp)
        else if (pidCode == ENGINE_COOLANT_TEMP)
        {
            uint32_t sensorValue = FakeSensorValueProvider();

            /**
             * Response parameters example:
             * pidRequest - we send the pid we received to identify it
             * numberOfBytes - the number of bytes this PID value has, using lookup table responseBytes
             * sensorValue - the value of the sensor - mock value in this example
             */
            ELMulator.writePidResponse(pidRequest, responseBytes[pidCode], sensorValue);
            return;
        }
        
        // Example doing some modification of mock sensor value 
        else if (pidCode == ENGINE_RPM)
        {
            uint16_t rpmValue = FakeSensorValueProvider() * 100;
            ELMulator.writePidResponse(pidRequest, responseBytes[pidCode], rpmValue);
            return;
        }

        // Respond to any other mode 01 request with a mock sensor value
        else
        { 
            ELMulator.writePidResponse(pidRequest, responseBytes[pidCode], FakeSensorValueProvider());
            return;
        }
    }
    
    else
    {
        /**
         * Not a mode 01 PID request. Report it as not supported (ie, "NO DATA");
         */
        ELMulator.writePidNotSupported();
    }
}
