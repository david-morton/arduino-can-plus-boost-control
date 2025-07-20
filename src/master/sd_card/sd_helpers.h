#pragma once

#include <Arduino.h>
#include <RTClib.h>
#include <SD.h>

/* ======================================================================
   CONSTANTS
   ====================================================================== */

constexpr auto FILENAME_BUFFER_SIZE = 16; // Size of the buffer for filenames

/* ======================================================================
   PIN CONSTANTS
   ====================================================================== */

const byte SD_CS_PIN = 7; // Chip select pin for the SD card breakout board
const byte SD_CD_PIN = 8; // Card detect pin for the SD card breakout board

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
