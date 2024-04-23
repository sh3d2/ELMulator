#ifndef ELMulator_OBDWiFiComm_h
#define ELMulator_OBDWiFiComm_h

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiServer.h>
#include "definitions.h"


class OBDWiFiComm
{
public:
    enum STATUS
    {
        IDLE = 0,
        READY = 1
    };

    OBDWiFiComm();

    ~OBDWiFiComm();

    void init(const String &deviceName);

    void writeEndOK();

    void writeEndERROR();

    /**
     * Response for unsupported PID command or missing sensor, respond "NO DATA".
     */
    void writeEndNoData();

    /**
     * Response for invalid AT command, or invalid PID or other bad input, respond "?"
     */
    void writeEndUnknown();

    void setToDefaults();

    void readData(String &rxData);

    void writeTo(uint8_t cChar);

    void writeTo(char const *string);

    void setEcho(bool echo);

    void writeEnd();

    bool isEchoEnable();

    void setLineFeeds(bool status);

    void setMemory(bool status);

    void setWhiteSpaces(bool status);

    void setHeaders(bool status);

    void setStatus(STATUS status);

    void writeEndPidTo(char const *string);

private:
    STATUS status;     // Operation status
    bool echoEnable;   // echoEnable command after received
    bool lineFeedEnable;
    bool memoryEnabled;
    bool whiteSpacesEnabled;
    bool headersEnabled;

    void addSpacesToResponse(const char *response, char string[]);

    WiFiClient client;
};

#endif