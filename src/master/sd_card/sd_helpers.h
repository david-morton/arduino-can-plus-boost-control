#pragma once

#include <Arduino.h>

/* ======================================================================
   PIN CONSTANTS
   ====================================================================== */

const byte SD_CS_PIN = 7; // Chip select pin for the SD card breakout board
const byte SD_CD_PIN = 8; // Card detect pin for the SD card breakout board

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern bool sdCardInserted;

/* ======================================================================
   FUNCTION PROTYPES
   ====================================================================== */

void initialiseSdBreakout();
void reportSdCardInfo();
void handleSdCardScheduledTasks();
