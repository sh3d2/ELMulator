#pragma once
#include <Arduino.h>
#include <ELMulator.h>
#include <definitions.h>

void setupSerial();
void handlePIDRequest(const String& pidRequest);
