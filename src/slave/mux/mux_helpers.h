#pragma once

#include <Arduino.h>

/* ======================================================================
   CONSTANTS
   ====================================================================== */

constexpr int DIGITAL_VOLTAGE_THRESHOLD = 500; // Threshold for digital logic read via analog pin

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void setupMux();
int  readAveragedMuxAnalogueChannel(byte channel, int samples, int delayUs);
bool readStableMuxDigitalChannelReading(byte channel, int samples, int delayUs);
