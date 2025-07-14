#include <ptScheduler.h>

#include "sd_helpers.h"

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// File logFile;
// bool logFileOpen = false;
// char currentLogFilename[32]; // Example: "log_20250712_203015.txt"

// void onEngineStart() {
//   // Called when engine RPM goes from 0 -> non-zero
//   generateTimestampedFilename(currentLogFilename);
//   logFile = SD.open(currentLogFilename, FILE_WRITE);
//   if (logFile) {
//     DEBUG_SD("Opened log file: %s", currentLogFilename);
//     logFileOpen = true;
//   } else {
//     DEBUG_SD("Failed to open log file!");
//   }
// }

// void logDataLine(const char *data) {
//   if (logFileOpen) {
//     logFile.println(data);
//     logFile.flush(); // Protect against sudden power loss
//   }
// }

// void onEngineShutdown() {
//   // Called when engine RPM = 0 and has stayed 0 for some timeout
//   if (logFileOpen) {
//     DEBUG_SD("Closing log file due to engine shutdown");
//     logFile.close();
//     logFileOpen = false;
//   }
// }


// unsigned long engineZeroRpmStart = 0;

// void checkForEngineShutdown(unsigned int currentRpm) {
//   if (currentRpm == 0) {
//     if (engineZeroRpmStart == 0) {
//       engineZeroRpmStart = millis();
//     } else if (millis() - engineZeroRpmStart > 5000) {
//       onEngineShutdown();  // Been 0 RPM for 5 seconds
//     }
//   } else {
//     engineZeroRpmStart = 0;  // Reset if engine is running again
//   }
// }
