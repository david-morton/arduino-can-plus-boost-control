#include <Arduino.h>
#include <RTClib.h>
#include <SD.h>
#include <SPI.h>
#include <ptScheduler.h>

#include "../../shared/debug_logging.h"
#include "../rtc/rtc_sensor.h"
#include "sd_helpers.h"
#include "sd_card.h"

/* ======================================================================
   SCHEDULED TASK OBJECTS
   ====================================================================== */

// ptScheduler ptWriteErrorLog       = ptScheduler(PT_TIME_500MS);
ptScheduler ptWriteTelemetryLog   = ptScheduler(PT_TIME_500MS);
// ptScheduler ptDetectEngineStopped = ptScheduler(PT_TIME_500MS);

// TODO: Look at how we start / stop logging based on engine state and the sdReadyToLogTelemetry flag.
// Most likely this wants to be on a scheduled task that checks the engine state and sets the flag accordingly.

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Initializes the SD card and prepares log files for use
bool sdCardInserted        = false;
bool sdReadyToLogTelemetry = false;
char telemetryLogFilename[FILENAME_BUFFER_SIZE];

void initialiseSdBreakout() {
  DEBUG_GENERAL("Initialising SD card breakout ...");

  pinMode(SD_CD_PIN, INPUT_PULLUP);
  delay(10); // Allow pin to settle

  if (digitalRead(SD_CD_PIN) == HIGH) {
    DEBUG_GENERAL("\t\tCard detected. Attempting to initialize...");

    if (SD.begin(SD_CS_PIN)) {
      DEBUG_GENERAL("\t\tSD card initialized successfully.");
      sdCardInserted = true;
      createSdLogFiles(); // Initialize log files
    } else {
      DEBUG_ERROR("\tSD.begin failed. Card initialization failed.");
      sdCardInserted = false;
    }
  } else {
    DEBUG_ERROR("\tNo SD card detected.");
    sdCardInserted = false;
  }
}

// Generates timestamped log files with file name format: "YYMMDDnn.xxx" where nn is
// a sequential number and xxx indicates the type of log (e.g. err for error). The
// default csv extension is used for telemetry logs as the most common use case.
void createSdLogFiles() {
  DateTime now = getRtcCurrentDateTime();

  if (generateNextAvailableLogFilename(telemetryLogFilename, sizeof(telemetryLogFilename), now, "CSV")) {
    File logFileTelemetry = SD.open(telemetryLogFilename, FILE_WRITE);
    if (logFileTelemetry) {
      bool headerWriteResult = writeSdTelemetryLogHeader(&logFileTelemetry); // Write header row for telemetry log
      if (headerWriteResult) {
        sdReadyToLogTelemetry = true;
        DEBUG_GENERAL("\t\tCreated telemetry log file with header: %s", telemetryLogFilename);
      } else {
        DEBUG_ERROR("\tFailed to write header row to telemetry log file: %s", telemetryLogFilename);
        sdReadyToLogTelemetry = false;
      }
      logFileTelemetry.close(); // Always close after creating
    } else {
      sdReadyToLogTelemetry = false;
      DEBUG_ERROR("\tFailed to open telemetry log file: %s", telemetryLogFilename);
    }
  } else {
    sdReadyToLogTelemetry = false;
    DEBUG_ERROR("\tFailed to find available telemetry log filename");
  }
}

// Return the next available log filename based on the current date and extension
bool generateNextAvailableLogFilename(char *buffer, size_t bufferSize, const DateTime &now, const char *extension) {
  for (int i = 0; i <= 99; ++i) {
    snprintf(buffer, bufferSize, "%02d%02d%02d%02d.%s",
             now.year() % 100, now.month(), now.day(), i, extension);

    if (!SD.exists(buffer)) {
      return true;
    }
  }
  return false;
}



// Execute scheduled tasks related to SD card operations
void handleSdCardScheduledTasks() {
  // Write telemetry data to the log file
  if (sdReadyToLogTelemetry && ptWriteTelemetryLog.call()) {
    writeSdTelemetryLogLine();
  }
}
