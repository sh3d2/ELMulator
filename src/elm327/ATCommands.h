#ifndef GTTURBOECU_ATCommands_h
#define GTTURBOECU_ATCommands_h


#include <Arduino.h>
#include "OBDSerialCommBT.h"

class ATCommands {

public:
    ATCommands(OBDSerialCommBT *connection);

    ~ATCommands();

    bool process(String string);

private:

    // Variables
    OBDSerialCommBT *connection;

    void ATD();

    void ATZ();

    void ATI();

    void ATEx(String x);

    void ATMx(String x);

    void ATLx(String x);

    void ATSx(String x);

    void ATHx(String x);

    void ATSPx(String x);

    void ATATx(String cmd);

    void ATPC();

    void ATDPN();

    void ATDESC();

    void processCommand(String command);

    bool isATCommand(String command);

};

#endif

