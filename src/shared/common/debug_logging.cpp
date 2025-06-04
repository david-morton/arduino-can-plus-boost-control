#include "debug_logging.h"

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */
unsigned long arduinoLoopExecutionPreviousExecutionMillis;

/* ======================================================================
   FUNCTION DEFINITIONS - FUNCTIONAL
   ====================================================================== */
// This function reads an analogue pin multiple times and returns the average value.
int getAveragedAnaloguePinReading(byte pin, int samples, int delayUs) {
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

/* ======================================================================
   FUNCTION DEFINITIONS - PERFORMANCE
   ====================================================================== */

// This function reports the execution frequency of the Arduino loop
void reportArduinoLoopRate(unsigned long *loopCount) {
  float loopFrequencyHz = (*loopCount / ((millis() - arduinoLoopExecutionPreviousExecutionMillis) / 1000));
  float loopExecutionMs = (millis() - arduinoLoopExecutionPreviousExecutionMillis) / *loopCount;
  DEBUG_PERFORMANCE("Loop execution frequency (Hz): %.2f or every %lu ms", loopFrequencyHz, loopExecutionMs);
  *loopCount                                  = 1;
  arduinoLoopExecutionPreviousExecutionMillis = millis();
}
