#pragma once

#include <Arduino.h>

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void setupMux();
int  readAveragedMuxAnalogueChannel(byte channel, int samples, int delayUs);
bool readStableMuxDigitalChannelReading(byte channel, int samples, int delayUs);
