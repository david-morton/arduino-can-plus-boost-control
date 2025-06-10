#include "debug_logging.h"

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */
unsigned long arduinoLoopExecutionPreviousExecutionMillis;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// This function reports the execution frequency of the Arduino loop
void reportArduinoLoopRate(unsigned long *loopCount) {
  float loopFrequencyHz = (*loopCount / ((millis() - arduinoLoopExecutionPreviousExecutionMillis) / 1000));
  float loopExecutionMs = (millis() - arduinoLoopExecutionPreviousExecutionMillis) / *loopCount;
  DEBUG_PERFORMANCE("Loop execution frequency (Hz): %.2f or every %lu ms", loopFrequencyHz, loopExecutionMs);
  *loopCount                                  = 1;
  arduinoLoopExecutionPreviousExecutionMillis = millis();
}
