#pragma once

/* ======================================================================
   STRUCTURES
   ====================================================================== */

// Structure to hold the alarm thresholds
struct AlarmThresholds {
  int ambientLuxMin;
  int engineRpmMax;
};

/* ======================================================================
   VARIABLES
   ====================================================================== */

// Default alarm thresholds
const AlarmThresholds alarmThresholds = {
    .ambientLuxMin = 20,
    .engineRpmMax  = 7500};

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void checkAndUpdateAlarmState();
