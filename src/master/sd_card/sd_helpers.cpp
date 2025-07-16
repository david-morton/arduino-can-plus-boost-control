#include <Arduino.h>
#include <RTClib.h>
#include <SD.h>
#include <SPI.h>
#include <ptScheduler.h>

#include "../../shared/debug_logging.h"
#include "../rtc/rtc_sensor.h"
#include "sd_card.h"
#include "sd_helpers.h"

/* ======================================================================
   SCHEDULED TASK OBJECTS
   ====================================================================== */

ptScheduler ptWriteTelemetryLog     = ptScheduler(PT_TIME_500MS);
ptScheduler ptCheckForEngineStopped = ptScheduler(PT_TIME_500MS);
ptScheduler ptFlushLogFiles         = ptScheduler(PT_TIME_5S);

/* ======================================================================
   VARIABLES
   ====================================================================== */

bool          globalHealthSdCardLogging                  = true; // Global flag to control SD card logging health
bool          sdCardInserted                             = false;
bool          sdReadyToLogTelemetry                      = false;
bool          sdLogTelemetry                             = false;
char          telemetryLogFilename[FILENAME_BUFFER_SIZE] = {0};
const int     ENGINE_STOPPED_RPM_THRESHOLD               = 2000; // RPM threshold to consider engine stopped
File          logFileTelemetry;
DateTime      rtcStartTime;
unsigned long millisAtRtcStart = 0;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Get current date and time from the RTC sensor
void updateRtcStartTime() {
  rtcStartTime     = getRtcCurrentDateTime();
  millisAtRtcStart = millis();
  DEBUG_GENERAL("\tRTC start time updated: %02d:%02d:%02d", rtcStartTime.hour(), rtcStartTime.minute(), rtcStartTime.second());
}

// Get ISO 8601 timestamp in the format "YYYY-MM-DDTHH:MM:SS.sss"
void getIso8601Timestamp(char *buffer, size_t bufferSize) {
  unsigned long elapsed = millis() - millisAtRtcStart;

  // Add elapsed seconds to the RTC base
  DateTime currentTime = rtcStartTime + TimeSpan(0, 0, 0, elapsed / 1000);
  int      millisPart  = elapsed % 1000;

  snprintf(buffer, bufferSize,
           "%04d-%02d-%02dT%02d:%02d:%02d.%03d",
           currentTime.year(), currentTime.month(), currentTime.day(),
           currentTime.hour(), currentTime.minute(), currentTime.second(),
           millisPart);
}

// Initialises the SD card breakout board and checks for card presence
void initialiseSdBreakout() {
  DEBUG_GENERAL("Initialising SD card breakout ...");

  pinMode(SD_CD_PIN, INPUT_PULLUP);
  delay(10); // Allow pin to settle

  if (digitalRead(SD_CD_PIN) == HIGH) {
    DEBUG_GENERAL("\t\tCard detected. Attempting to initialize...");

    if (SD.begin(SD_CS_PIN)) {
      DEBUG_GENERAL("\t\tSD card initialized successfully.");
      sdCardInserted = true;
      updateRtcStartTime(); // Update RTC start time on successful SD card initialization
      createSdLogFiles();   // Initialize log files
    } else {
      DEBUG_ERROR("\tSD.begin failed. Card initialization failed.");
      sdCardInserted            = false;
      globalHealthSdCardLogging = false; // Disable logging if SD card initialization fails
    }
  } else {
    DEBUG_ERROR("\tNo SD card detected.");
    sdCardInserted            = false;
    globalHealthSdCardLogging = false; // Disable logging if no card is detected
  }
}

// Generates timestamped log files with file name format: "YYMMDDnn.xxx"
void createSdLogFiles() {
  DateTime now = getRtcCurrentDateTime();

  if (generateNextAvailableLogFilename(telemetryLogFilename, sizeof(telemetryLogFilename), now, "CSV")) {
    logFileTelemetry = SD.open(telemetryLogFilename, FILE_WRITE);
    if (logFileTelemetry) {
      bool headerWriteResult = writeSdTelemetryLogHeader(&logFileTelemetry); // Write header row
      if (headerWriteResult) {
        sdReadyToLogTelemetry = true;
        DEBUG_GENERAL("\t\tCreated telemetry log file with header: %s", telemetryLogFilename);
      } else {
        DEBUG_ERROR("\tFailed to write header to telemetry log file: %s", telemetryLogFilename);
        logFileTelemetry.close();
        logFileTelemetry      = File();
        sdReadyToLogTelemetry = false;
      }
    } else {
      DEBUG_ERROR("\tFailed to open telemetry log file: %s", telemetryLogFilename);
      sdReadyToLogTelemetry = false;
    }
  } else {
    DEBUG_ERROR("\tFailed to find available telemetry log filename");
    sdReadyToLogTelemetry = false;
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

// Flush and close a log file safely
void flushAndCloseFile(File &file, const char *label) {
  if (file) {
    file.flush();
    file.close();
    file = File(); // Reset to null state
    DEBUG_SD("Flushed and closed %s file.", label);
  } else {
    DEBUG_SD("%s file was not open, nothing to close.", label);
  }
}

// Handle engine state changes for telemetry logging
void handleEngineStateChange() {
  if (currentEngineSpeedRpm < ENGINE_STOPPED_RPM_THRESHOLD && sdLogTelemetry == true) {
    sdLogTelemetry = false;
    DEBUG_SD("Engine stopped. Stopping telemetry logging.");
    flushAndCloseFile(logFileTelemetry, "Telemetry");
  } else if (currentEngineSpeedRpm >= ENGINE_STOPPED_RPM_THRESHOLD && sdLogTelemetry == false) {
    DEBUG_SD("Engine running at %d RPM. Starting telemetry logging.", currentEngineSpeedRpm);
    // Reopen file if needed
    if (!logFileTelemetry) {
      logFileTelemetry = SD.open(telemetryLogFilename, FILE_WRITE);
      if (!logFileTelemetry) {
        sdLogTelemetry = false;
        DEBUG_ERROR("Failed to reopen telemetry log file: %s", telemetryLogFilename);
      } else {
        sdLogTelemetry = true;
        DEBUG_SD("Reopened telemetry log file: %s", telemetryLogFilename);
      }
    } else {
      sdLogTelemetry = true; // Already open
      DEBUG_SD("Telemetry log file already open: %s", telemetryLogFilename);
    }
  }
}

// Execute scheduled tasks related to SD card operations
void handleSdCardScheduledTasks() {
  // Write telemetry data to the log file
  if (sdReadyToLogTelemetry && sdLogTelemetry && ptWriteTelemetryLog.call()) {
    writeSdTelemetryLogLine();
  }

  // Flush log files if required
  if (sdLogTelemetry && ptFlushLogFiles.call()) {
    logFileTelemetry.flush();
  }

  // Check if the engine has stopped and handle logging accordingly
  if (ptCheckForEngineStopped.call()) {
    handleEngineStateChange();
  }
}
