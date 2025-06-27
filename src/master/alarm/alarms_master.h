#pragma once

/* ======================================================================
   STRUCTURES
   ====================================================================== */

// Structure to hold the critical alarm thresholds
typedef struct {
  int engineTemperature;
} AlarmThresholdsCritical;

// Structure to hold the non-critical alarm thresholds
typedef struct {
  int pingLossPercent;
} AlarmThresholdsWarning;

/* ======================================================================
   VARIABLES
   ====================================================================== */

// Critical alarm thresholds
const AlarmThresholdsCritical alarmThresholdsCritical = {
    .engineTemperature = 115};

// Warning alarm thresholds
const AlarmThresholdsWarning alarmThresholdsWarning = {
    .pingLossPercent = 25};

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void updateLocalCriticalAlarmState();
void updateLocalWarningAlarmState();
void handleAllAlarmStatesMaster();
