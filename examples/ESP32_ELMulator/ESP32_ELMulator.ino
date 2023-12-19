#include "ESP32_ELMulator.h"

String deviceName = "ELMULATOR";
bool isCycleUp = true;
uint32_t cycle = 0;
ELMulator ELMulator;


void setup() 
{
    setupSerial();
    ELMulator.init(deviceName);

    // Set up PIDs 0x00 to 0x65 
    for (int i = 0; i < 102; i++)
    {
        ELMulator.registerMode01Pid(i);
    }
       
    ELMulator.registerMode01MILResponse("4101830000");
    ELMulator.registerMode03Response("43010341\r\n43010123\r\n43010420");
}
 
void loop() 
{
    String pidRequest = ELMulator.readPidRequest();
    handleRequest(pidRequest);
} 

void setupSerial() 
{// init serial communication, is only need if there's a need
    //  to read the debug info on serial port of the arduino
    Serial.begin(115200);
    Serial.println("ELMulator starting up . . .");
}


/**
 * Fake some data
 * start counter = 0
 * every call to getFakeSensorData, will increment the counter to a max of 255 (0xff)
 * When 0xff is reached the cycle is reverted
 * and the counter will decrement by one until 0 is reached
 */
uint32_t FakeSensorValueProvider() {

    if(isCycleUp) {
        cycle++;
        if(cycle == 0xff) {
            isCycleUp = false;
        }
    } else {
        cycle--;
        if (cycle == 0) {
            isCycleUp = true;
        }
    }

    return cycle;
}

void handleRequest(String pidRequest)
{
     /**
     * After receiving a PID(sensor) request
     * read the sensor value (in this case is a fake value)
     *
     * and send a response containing the sensor value
     */
    
    if (!strncmp(pidRequest.c_str(), "01", strlen("01")))        //Check for mode 01 request
    {
       Serial.print("pidRequest (1): "); Serial.println(pidRequest); 
        if (pidRequest.length() > 4)
        {
            pidRequest = pidRequest.substring(0, 4);            //remove num_responses value if present
        } 
        Serial.print("pidRequest: "); Serial.println(pidRequest);
        uint16_t hexCommand = strtoul(pidRequest.c_str(), NULL, HEX);
        uint8_t pidCode = ELMulator.getPidCodeOnly(hexCommand);
        
        // char requestPID[3] = {0};
        // sprintf(requestPID, &pidRequest[2]);
        
        
        //uint8_t pidCode = pidRequest.substring(pidRequest.length() - 2, pidRequest.length()).toInt();

        // uint8_t pidCode = (uint8_t)strtol(requestPID, NULL, DEC);
        
        // Handle any requests for which we have specific data, either from setup or via a sensor

        /**
         * 01A6 Odometer value provided in setup()
         */
        if (pidCode == ODOMETER) 
        {
            ELMulator.writePidResponse(pidRequest, 4, 1234567);
            return;
        }

        else if (pidRequest.equalsIgnoreCase("0105")) 
        { 
            uint8_t numberOfBytes = 1;
            uint32_t sensorValue = FakeSensorValueProvider();

            /**
             * Response parameters example:
             * pidRequest - we send the pid we received to identify it
             * numberOfBytes - the number of bytes this PID value has, see OBDII PID specifications
             * sensorValue - the value of the sensor
             */
            ELMulator.writePidResponse(pidRequest, numberOfBytes, sensorValue);
            return;
        }
        else if (pidRequest.equalsIgnoreCase("010C")) 
        {
            uint16_t rpmValue = FakeSensorValueProvider() * 100;
            ELMulator.writePidResponse(pidRequest, responseBytes[pidCode], rpmValue);
            return;
        }
        /**
         * 0170 VIN
         */
        else if (pidRequest.equalsIgnoreCase("0170")) {
            ELMulator.writePidResponse(pidRequest, 9, FakeSensorValueProvider());
            return;
        }

        else
        {   // Respondd to any other mode 01 request with a fake sensor value
            
            ELMulator.writePidResponse(pidRequest, responseBytes[pidCode], FakeSensorValueProvider());
            return;
        }
    }
    else 
    {
        /**
         * If pid not implemented, report it as not implemented
         */
        ELMulator.writePidNotSupported();
    }

}
