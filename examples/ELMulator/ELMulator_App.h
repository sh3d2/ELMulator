#pragma once
#include <Arduino.h>
#include <ELMulator.h>

void setupSerial();
void setupELMSim();
void processRequest(String pidRequest);

/**
 * Fake some data
 * start counter = 0
 * every call to getFakeSensorData, will increment the counter to a max of 255 (0xff)
 * When 0xff is reached the cycle is reverted
 * and the counter will decrement by one until 0 is reached
 */
uint32_t FakeSensorValueProvider();