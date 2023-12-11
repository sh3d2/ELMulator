#include "OBDSerialCommBT.h"


OBDSerialCommBT::OBDSerialCommBT() {
  
}

OBDSerialCommBT::~OBDSerialCommBT() {
    operator delete(serial);
}

void OBDSerialCommBT::init(String deviceName) {
    Serial.println("Starting BT . . .");
    serial = new BluetoothSerial();
    //delay(2000);
    serial->begin(deviceName, false);
    setToDefaults();

}

void OBDSerialCommBT::writeEnd() {

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


void OBDSerialCommBT::writeEndOK() {
    writeTo("OK");
    writeEnd();
}

void OBDSerialCommBT::writeEndERROR() {
    writeTo("ERROR");
    writeEnd();
}

void OBDSerialCommBT::writeEndNoData() {
    writeTo("NO DATA");
    writeEnd();
}

void OBDSerialCommBT::writeEndUnknown() {
    writeTo("?");
    writeEnd();
}

void OBDSerialCommBT::setToDefaults() {
    setEcho(true);
    setStatus(READY);
    setWhiteSpaces(true);
    setHeaders(false);
    setLineFeeds(true);
    setMemory(false);
}

void OBDSerialCommBT::writeTo(char const *response) {
    serial->print(response);
}


void OBDSerialCommBT::writeTo(uint8_t cChar) {
    serial->print(cChar);
}

void OBDSerialCommBT::writeEndPidTo(char const *response) {
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

String OBDSerialCommBT::readData() {
    serial->flush(); // temp remove this
    String rxData = serial->readStringUntil(SERIAL_END_CHAR);
    if (isEchoEnable()) {
        writeTo(rxData.c_str());
    }
    return rxData;
}

void OBDSerialCommBT::setBaudRate(uint32_t rate) {
    this->boudRate = rate;
}

long OBDSerialCommBT::getBaudRate() {
    return boudRate;
}

bool OBDSerialCommBT::isEchoEnable() {
    return echoEnable;
}

void OBDSerialCommBT::setEcho(bool echo) {
    this->echoEnable = echo;
}

void OBDSerialCommBT::setStatus(STATUS status) {
    this->status = status;
}

void OBDSerialCommBT::setLineFeeds(bool status) {
    this->lineFeedEnable = status;
}

void OBDSerialCommBT::setMemory(bool status) {
    this->memoryEnabled = status;
}

void OBDSerialCommBT::setWhiteSpaces(bool status) {
    this->whiteSpacesEnabled = status;
}

void OBDSerialCommBT::setHeaders(bool status) {
    this->headersEnabled = status;
}

void OBDSerialCommBT::addSpacesToResponse(const char *response, char spacedRes[]) {
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
