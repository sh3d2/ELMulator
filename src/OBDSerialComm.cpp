#include "OBDSerialComm.h"
#include "definitions.h"


OBDSerialComm::OBDSerialComm(uint32_t baudRate, uint8_t rxPin, uint8_t txPin) {
    rx = rxPin;
    tx = txPin;
    this->baudRate = baudRate;
    
}

OBDSerialComm::~OBDSerialComm() {
    operator delete(serial);
}

void OBDSerialComm::init(const String& deviceName) {
    Serial.println("Starting BT . . .");
    serial = new SoftwareSerial(rx, tx);
    //delay(2000);
    serial->begin(baudRate);
    setToDefaults();

}

void OBDSerialComm::writeEnd() {

    // 1 - write carriage return
    writeTo("\r");

    // 2- (optional ) write linefeed
    if (lineFeedEnable) {
        writeTo("\n");
    }

    // 3 - Write prompt
    writeTo(">");

    serial->flush();
};


void OBDSerialComm::writeEndOK() {
    writeTo("OK");
    writeEnd();
}

void OBDSerialComm::writeEndERROR() {
    writeTo("ERROR");
    writeEnd();
}

void OBDSerialComm::writeEndNoData() {
    writeTo("NO DATA");
    writeEnd();
}

void OBDSerialComm::writeEndUnknown() {
    writeTo("?");
    writeEnd();
}

void OBDSerialComm::setToDefaults() {
    setEcho(true);
    setStatus(READY);
    setWhiteSpaces(true);
    setHeaders(false);
    setLineFeeds(true);
    setMemory(false);
}

void OBDSerialComm::writeTo(char const *response) {
    serial->print(response);
}


void OBDSerialComm::writeTo(uint8_t cChar) {
    serial->print(cChar);
}

void OBDSerialComm::writeEndPidTo(char const *response) {
    if (whiteSpacesEnabled) {
        uint8_t len = strlen(response);
        char spacedResponse[len + len / 2 + 1];
        addSpacesToResponse(response, spacedResponse);
        writeTo(spacedResponse);
    } else {
        writeTo(response);
    }
    writeEnd();
}

void OBDSerialComm::readData(String& rxData) {
    serial->flush(); // temp remove this
    rxData = serial->readStringUntil(SERIAL_END_CHAR);
    if (isEchoEnable()) {
        writeTo(rxData.c_str());
    }
    //return rxData;
}

void OBDSerialComm::setBaudRate(uint32_t rate) {
    this->baudRate = rate;
}

long OBDSerialComm::getBaudRate() {
    return this->baudRate;
}

bool OBDSerialComm::isEchoEnable() {
    return this->echoEnable;
}

void OBDSerialComm::setEcho(bool echo) {
    this->echoEnable = echo;
}

void OBDSerialComm::setStatus(STATUS status) {
    this->status = status;
}

void OBDSerialComm::setLineFeeds(bool status) {
    this->lineFeedEnable = status;
}

void OBDSerialComm::setMemory(bool status) {
    this->memoryEnabled = status;
}

void OBDSerialComm::setWhiteSpaces(bool status) {
    this->whiteSpacesEnabled = status;
}

void OBDSerialComm::setHeaders(bool status) {
    this->headersEnabled = status;
}

void OBDSerialComm::addSpacesToResponse(const char *response, char spacedRes[]) {
    uint8_t len = strlen(response);
    int j = 0;
    for (int i = 0; i < len;) {
        *(spacedRes + j++) = *(response + i++);
        *(spacedRes + j++) = *(response + i++);
        if (i < len) {
            *(spacedRes + j++) = 0x20;
        }
    }
    *(spacedRes + j) = '\0';
}
