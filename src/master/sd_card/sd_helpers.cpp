#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <ptScheduler.h>

#include "../../shared/debug_logging.h"
#include "sd_helpers.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

bool sdCardInserted = false;

Sd2Card  card;
SdVolume volume;
SdFile   root;

/* ======================================================================
   SCHEDULED TASK OBJECTS
   ====================================================================== */

ptScheduler ptWriteErrorLog       = ptScheduler(PT_TIME_500MS);
ptScheduler ptWriteTelemetryLog   = ptScheduler(PT_TIME_500MS);
ptScheduler ptDetectEngineStopped = ptScheduler(PT_TIME_500MS);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Initializes the SD card and prints some debug information
void initialiseSdBreakout() {
  DEBUG_GENERAL("Initialising SD card breakout ...");

  pinMode(SD_CD_PIN, INPUT_PULLUP);

  // Manually deassert all other SPI devices BEFORE init
  // pinMode(3, OUTPUT);
  // digitalWrite(3, HIGH);

  // pinMode(5, OUTPUT);
  // digitalWrite(5, HIGH);

  // pinMode(10, OUTPUT);
  // digitalWrite(10, HIGH);

  // SPI.begin();
  // delay(10); // Let SPI lines settle

  if (digitalRead(SD_CD_PIN) == HIGH) {
    DEBUG_GENERAL("\t\tCard detected. Attempting to initialize...");

    if (card.init(SPI_HALF_SPEED, SD_CS_PIN)) {
      DEBUG_GENERAL("\t\tSD card initialized successfully.");
      sdCardInserted = true;
      // Output card information
      reportSdCardInfo();
    } else {
      DEBUG_ERROR("\tSD card initialization failed.");
      DEBUG_ERROR("\tError code: %d, Error data: %d", card.errorCode(), card.errorData());
      sdCardInserted = false;
    }
  } else {
    DEBUG_ERROR("\tNo SD card detected.");
    sdCardInserted = false;
  }
}

// Reports SD card information such as size and type
void reportSdCardInfo() {
  const char *typeString;

  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      typeString = "SD1";
      break;
    case SD_CARD_TYPE_SD2:
      typeString = "SD2";
      break;
    case SD_CARD_TYPE_SDHC:
      typeString = "SDHC";
      break;
    default:
      typeString = "Unknown";
      break;
  }

  DEBUG_GENERAL("\t\tCard type: %s", typeString);

  if (!volume.init(card)) {
    DEBUG_ERROR("\t\tCould not find FAT16/FAT32 partition. Is the card formatted?");
    return;
  }

  uint32_t clusters         = volume.clusterCount();
  uint32_t blocksPerCluster = volume.blocksPerCluster();
  uint32_t totalBlocks      = clusters * blocksPerCluster;
  uint32_t volumeMB         = totalBlocks / 2 / 1024;
  float    volumeGB         = (float)volumeMB / 1024.0;

  DEBUG_GENERAL("\t\tFAT type: FAT%u", volume.fatType());
  DEBUG_GENERAL("\t\tVolume size: %lu MB / %.2f GB", volumeMB, volumeGB);
}

// Execute scheduled tasks related to SD card operations
void handleSdCardScheduledTasks() {
}
