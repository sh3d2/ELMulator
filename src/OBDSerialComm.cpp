#include "OBDSerialComm.h"
#include "definitions.h"


OBDSerialComm::OBDSerialComm() {
    
}

OBDSerialComm::~OBDSerialComm() {
    operator delete(serial);
}

void OBDSerialComm::init(String deviceName) {
    Serial.println("Starting BT . . .");
    serial = new BluetoothSerial();
    //delay(2000);
    serial->begin(deviceName, false);
    setToDefaults();

}

void OBDSerialComm::writeEnd() {

    // 1 - write carriage return
    //    writeTo(0x0D);
    writeTo("\n");
    //    writeTo(13);


    // 2- (optional ) write linefeed
    if (lineFeedEnable) {
        writeTo("\r");
        // writeTo(10);
    }

    // 3 - Write prompt
    //    writeTo(0x3E);
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

String OBDSerialComm::readData() {
    serial->flush(); // temp remove this
    String rxData = serial->readStringUntil(SERIAL_END_CHAR);
    if (isEchoEnable()) {
        writeTo(rxData.c_str());
    }
    return rxData;
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
