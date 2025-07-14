#include <Arduino.h>
#include <RTClib.h>
#include <SD.h>
#include <SPI.h>
#include <ptScheduler.h>

#include "../../shared/debug_logging.h"
#include "../rtc/rtc_sensor.h"
#include "sd_helpers.h"

/* ======================================================================
   SCHEDULED TASK OBJECTS
   ====================================================================== */

ptScheduler ptWriteErrorLog       = ptScheduler(PT_TIME_500MS);
ptScheduler ptWriteTelemetryLog   = ptScheduler(PT_TIME_500MS);
ptScheduler ptDetectEngineStopped = ptScheduler(PT_TIME_500MS);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Initializes the SD card and prepares log files for use
bool sdCardInserted        = false;
bool sdLogCreatedTelemetry = false;
char telemetryLogFilename[FILENAME_BUFFER_SIZE];
int  telemetryLogIndex = 0;

void initialiseSdBreakout() {
  DEBUG_GENERAL("Initialising SD card breakout ...");

  pinMode(SD_CD_PIN, INPUT_PULLUP);
  delay(10); // Allow pin to settle

  if (digitalRead(SD_CD_PIN) == HIGH) {
    DEBUG_GENERAL("\t\tCard detected. Attempting to initialize...");

    if (SD.begin(SD_CS_PIN)) {
      DEBUG_GENERAL("\t\tSD card initialized successfully.");
      sdCardInserted = true;
      // reportSdCardInfo(); // Report card info
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

// Reports information about the SD card, such as type and size
// void reportSdCardInfo() {
//   Sd2Card  card;
//   SdVolume volume;

//   if (!card.init(SPI_HALF_SPEED, SD_CS_PIN)) {
//     DEBUG_ERROR("\tFailed to access SD card for info.");
//     return;
//   }

//   const char *typeString;
//   switch (card.type()) {
//     case SD_CARD_TYPE_SD1:
//       typeString = "SD1";
//       break;
//     case SD_CARD_TYPE_SD2:
//       typeString = "SD2";
//       break;
//     case SD_CARD_TYPE_SDHC:
//       typeString = "SDHC";
//       break;
//     default:
//       typeString = "Unknown";
//       break;
//   }

//   DEBUG_GENERAL("\t\tSD Card type: %s", typeString);

//   if (!volume.init(card)) {
//     DEBUG_ERROR("\tCould not find FAT16/FAT32 partition. Is the card formatted?");
//     return;
//   }

//   uint32_t clusters         = volume.clusterCount();
//   uint32_t blocksPerCluster = volume.blocksPerCluster();
//   uint32_t totalBlocks      = clusters * blocksPerCluster;
//   uint32_t volumeMB         = totalBlocks / 2 / 1024;
//   float    volumeGB         = (float)volumeMB / 1024.0;

//   DEBUG_GENERAL("\t\tFAT type: FAT%u", volume.fatType());
//   DEBUG_GENERAL("\t\tVolume size: %lu MB / %.2f GB", volumeMB, volumeGB);
// }

// Generates timestamped log files with file name format: "YYMMDDnn.xxx" where nn is
// a sequential number and xxx indicates the type of log (e.g. tel for telemetry).
void createSdLogFiles() {
  DateTime now = getRtcCurrentDateTime();

  if (generateNextAvailableLogFilename(telemetryLogFilename, sizeof(telemetryLogFilename), now, "TEL")) {
    DEBUG_SD("About to open telemetry log file: %s", telemetryLogFilename);
    File logFileTelemetry = SD.open(telemetryLogFilename, FILE_WRITE);
    if (logFileTelemetry) {
      logFileTelemetry.println("Log started");
      logFileTelemetry.close(); // Always close after creating
      sdLogCreatedTelemetry = true;
      DEBUG_GENERAL("\t\tCreated telemetry log file: %s", telemetryLogFilename);
    } else {
      sdLogCreatedTelemetry = false;
      DEBUG_ERROR("\tFailed to open telemetry log file: %s", telemetryLogFilename);
    }
  } else {
    sdLogCreatedTelemetry = false;
    DEBUG_ERROR("\tFailed to find available telemetry log filename");
  }
}

// Return the next available log filename based on the current date and extension
bool generateNextAvailableLogFilename(char *buffer, size_t bufferSize, const DateTime &now, const char *extension) {
  for (int i = 0; i <= 99; ++i) {
    snprintf(buffer, bufferSize, "%02d%02d%02d%02d.%s",
             now.year() % 100, now.month(), now.day(), i, extension);

    // Compare buffer with "25071500.TEL" at hex level
    const char *reference = "25071500.TEL";
    Serial.println("Hex comparison between buffer and reference:");
    for (size_t idx = 0; idx < strlen(reference); ++idx) {
      char bufChar = buffer[idx];
      char refChar = reference[idx];
      char msg[80];
      snprintf(msg, sizeof(msg), "Index %2d: buffer=0x%02X ('%c'), reference=0x%02X ('%c')\n",
               (int)idx, (unsigned char)bufChar, bufChar, (unsigned char)refChar, refChar);
      Serial.print(msg);
    }

    if (!SD.exists(buffer)) {
      DEBUG_SD("\t\tLog filename available: %s", buffer);
      return true;
    } else {
      DEBUG_SD("\t\tFile already exists: %s", buffer);
    }
  }

  return false;
}

// Execute scheduled tasks related to SD card operations
void handleSdCardScheduledTasks() {
}
