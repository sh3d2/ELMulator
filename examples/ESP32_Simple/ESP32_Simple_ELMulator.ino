#include "ESP32_Simple_ELMulator.h"

String deviceName = "ELMULATOR";
ELMulator ELMulator;

void setup()
{
    // Simplest implementation of ELMulator.
    // Supports all Mode 01 PIDs and responds with mock data values
    // Typical use case is for testing applications to work with ELM327 devices
    Serial.begin(115200);
    Serial.println("Starting ELMulator...");
    ELMulator.init(deviceName, true); // Initialize with support for all PIDs 0x00 to 0x65, providing mock data
    ELMulator.begin();                // ELMulator will run continuously, listening for and respondng to requests
}

void loop() {}
