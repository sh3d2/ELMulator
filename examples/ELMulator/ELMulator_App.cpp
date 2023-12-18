#include "ELMulator_App.h"

String deviceName = "ELMULATOR";
bool isCycleUp = true;
uint32_t cycle = 0;
ELMulator ELMulator;


void setup() 
{
    setupSerial();
    setupELMSim();
}
 
void loop() 
{
    String pidRequest = ELMulator.readPidRequest();
    processRequest(pidRequest);
} 

void setupSerial() 
{// init serial communication, is only need if there's a need
    //  to read the debug info on serial port of the arduino
    Serial.begin(115200);
    Serial.println("ELMulator starting up . . .");
}

void setupELMSim() 
{
    ELMulator.init(deviceName);
    ELMulator.registerMode01Pid(0x05);
    ELMulator.registerMode01Pid(0x0B);
    ELMulator.registerMode01Pid(0x0C);
    ELMulator.registerMode01Pid(0x0D);
    ELMulator.registerMode01Pid(0x33);
    ELMulator.registerMode01Pid(0x46);
    ELMulator.registerMode01Pid(0x5C);
    ELMulator.registerMode01Pid(0x70);
    ELMulator.registerMode01Pid(0xA6);
    
    ELMulator.registerMode01MILResponse("4101830000");
    ELMulator.registerMode03Response("43010341\r\n43010123\r\n43010420");
}

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

void processRequest(String pidRequest)
{
     /**
     * After receiving a PID(sensor) request
     * read the sensor value (in this case is a fake value)
     *
     * and send a response containing the sensor value
     */



    /**
     * 0105 Engine coolant temperature
    */
    if (pidRequest.equalsIgnoreCase("0105")) {

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

    /**
     * 010B intake manifold abs pressure
     */
    if (pidRequest.equalsIgnoreCase("010B")) {
        ELMulator.writePidResponse(pidRequest, 1, FakeSensorValueProvider());
        return;
    }

    /**
     * 010C engine rpm
     */
    if (pidRequest.equalsIgnoreCase("010C")) {
        uint16_t rpmValue = FakeSensorValueProvider() * 100;

        // Note: this time the PID value has two bytes
        ELMulator.writePidResponse(pidRequest, 2, rpmValue);
        return;
    }

    /**
     * 0D Vehicle speed
     */
    if (pidRequest.equalsIgnoreCase("010D")) {
        ELMulator.writePidResponse(pidRequest, 1, FakeSensorValueProvider());
        return;
    }

    /**
     * 33 Absolute Barometric Pressure
     */
    if (pidRequest.equalsIgnoreCase("0133")) {
        ELMulator.writePidResponse(pidRequest, 1, FakeSensorValueProvider());
        return;
    }

    /**
     * 0146 Ambient air temperature
     */
    if (pidRequest.equalsIgnoreCase("0146")) {
        ELMulator.writePidResponse(pidRequest, 1, FakeSensorValueProvider());
        return;
    }

    /**
     * 015C Engine oil temperature
     */
    if (pidRequest.equalsIgnoreCase("015C")) {
        ELMulator.writePidResponse(pidRequest, 1, FakeSensorValueProvider());
        return;
    }

    /**
     * 0170 VIN
     */
    else if (pidRequest.equalsIgnoreCase("0170")) {
        ELMulator.writePidResponse(pidRequest, 9, FakeSensorValueProvider());
        return;
    }

    /**
     * 01A6 Odometer
     */
    else if (pidRequest.equalsIgnoreCase("01A6")) {
        ELMulator.writePidResponse(pidRequest, 4, 1234567);
        return;
    }


    /**
     * If pid not implemented, report it as not implemented
     */
    ELMulator.writePidNotSupported();

}
