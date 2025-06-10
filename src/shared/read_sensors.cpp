#include "Arduino.h"

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// This function reads an analogue pin multiple times and returns the average value.
int readAveragedAnaloguePinReading(byte pin, int samples, int delayUs) {
  int totalReadings = 0;

  for (int i = 0; i < samples; i++) {
    if (delayUs != 0) {
      delayMicroseconds(delayUs);
    }
    totalReadings += analogRead(pin);
  }

  // Calculate average of the readings
  int averageReading = totalReadings / static_cast<float>(samples);
  return averageReading;
}
