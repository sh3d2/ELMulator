//
// Created by dragon on 12-05-2016.
//

#ifndef GTTURBOECU_PIDPROCESSOR_H
#define GTTURBOECU_PIDPROCESSOR_H

#include <Arduino.h>
#include <WString.h>
#include <Print.h>
#include "definitions.h"
#include "OBDSerialCommBT.h"

class PidProcessor {

public:

    PidProcessor(OBDSerialCommBT *connection);

    bool process(String string);

    bool registerMode01Pid(uint32_t pid);

    bool registerMode03Response(String response);

    void writePidResponse(String requestPid, uint8_t numberOfBytes, uint32_t value);

private:

    OBDSerialCommBT *_connection;
    
    uint32_t pidMode01Supported[N_MODE01_INTERVALS];

    String dtcResponse;

    bool isSupportedPidRequest(uint8_t pid);

    uint32_t getSupportedPids(uint8_t pidcode);

    bool isMode01(String command);
    bool isMode03(String command);

    uint8_t getPidIntervalId(uint8_t pidcode);

    uint8_t getPidIntervalIndex(uint8_t pidcode);

    void setPidBit(uint8_t pid);

    uint8_t getPidBitPosition(uint8_t pidcode);

    String convertToPidResponse(String pid);

    void getFormattedResponse(char *response, uint8_t totalNumberOfChars, String pid, uint32_t value);

    void resetPidMode01Array();

    uint8_t getPidCodeOnly(uint16_t hexCommand);

};


#endif //GTTURBOECU_PIDPROCESSOR_H
