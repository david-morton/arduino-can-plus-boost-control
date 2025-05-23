#include "general_helpers.h"

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */
const int millisWithoutSerialCommsBeforeFault = 1000; // How long is ms without serial comms from the master before we declare a critical alarm

/* ======================================================================
   FUNCTION: Get average readings from analogue pin
   ====================================================================== */
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
   FUNCTION: Report Arduino loop rate information
   ====================================================================== */
unsigned long arduinoLoopExecutionPreviousExecutionMillis;

void reportArduinoLoopRate(unsigned long *loopCount) {
  float loopFrequencyHz = (*loopCount / ((millis() - arduinoLoopExecutionPreviousExecutionMillis) / 1000));
  float loopExecutionMs = (millis() - arduinoLoopExecutionPreviousExecutionMillis) / *loopCount;
  DEBUG_LOOP_INFO("Loop execution frequency (Hz): %.2f or every %lu ms", loopFrequencyHz, loopExecutionMs);
  *loopCount                                  = 1;
  arduinoLoopExecutionPreviousExecutionMillis = millis();
}
