#pragma once
#include <Arduino.h>
#include <ELMulator.h>
#include <definitions.h>

void setupSerial();
void handleRequest(String pidRequest);
uint32_t FakeSensorValueProvider();
bool isMode01(String command);
bool isMode03(String command);
bool isMode01MIL(String command);