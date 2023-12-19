#pragma once
#include <Arduino.h>
#include <ELMulator.h>
#include <definitions.h>

void setupSerial();
void handleRequest(String pidRequest);
uint32_t FakeSensorValueProvider();