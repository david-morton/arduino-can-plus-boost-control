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
  unsigned long elapsedMs       = millis() - arduinoLoopExecutionPreviousExecutionMillis;
  double        loopFrequencyHz = (*loopCount * 1000.0) / elapsedMs; // Hz = cycles / seconds
  unsigned long loopExecutionMs = elapsedMs / *loopCount;            // ms per loop

  DEBUG_PERFORMANCE("Loop execution frequency (Hz): %.2f or every %lu ms", loopFrequencyHz, loopExecutionMs);

  *loopCount                                  = 1;
  arduinoLoopExecutionPreviousExecutionMillis = millis();
}
