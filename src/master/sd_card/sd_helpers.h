#pragma once

#include <Arduino.h>
#include <RTClib.h>
#include <SD.h>

/* ======================================================================
   CONSTANTS
   ====================================================================== */

constexpr auto FILENAME_BUFFER_SIZE = 16; // Size of the buffer for filenames

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern bool          globalHealthSdCardLogging;
extern File          logFileTelemetry;
extern DateTime      rtcStartTime;
extern unsigned long millisAtRtcStart;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

bool generateNextAvailableLogFilename(char *buffer, size_t bufferSize, const DateTime &now, const char *extension);
void createSdLogFiles();
void flushAndCloseLogFile();
void generateLogTimestamp(char *buffer, size_t bufferSize);
void handleEngineStateChange();
void handleSdCardScheduledTasks();
void initialiseSdBreakout();
void updateRtcStartTime();
