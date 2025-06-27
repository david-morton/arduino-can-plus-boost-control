#include <Arduino.h>

#include "../../shared/alarm/alarm_helpers.h"
#include "../../shared/debug_logging.h"
#include "../../shared/variables_programmatic.h"
#include "../../shared/variables_vehicle_parameters.h"
#include "alarms_master.h"

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Check all current critical alarm conditions, and update the alarm state accordingly
void updateLocalCriticalAlarmState() {
  if (currentEngineTempCelcius < alarmThresholdsCritical.engineTemperature) {
    localAlarmCriticalState = true;
  } else {
    localAlarmCriticalState = false;
  }
}

// Check all current warning alarm conditions, and update the alarm state accordingly
void updateLocalWarningAlarmState() {
  if (pingLossPercent > alarmThresholdsWarning.pingLossPercent) {
    localAlarmWarningState = true;
  } else {
    localAlarmWarningState = false;
  }
}

// Take the necessary actions based on the current global alarm states
void handleAllAlarmStatesMaster() {
  updateLocalCriticalAlarmState();
  updateLocalWarningAlarmState();
  evaluateGlobalAlarmStates();

  // If the global critical alarm state is active, enable the buzzer
  if (globalAlarmCriticalState) {
    // TAKE ACTIONS SPECIFIC TO MASTER HERE LIKE SETTING BOOST TARGETS
  } else if (globalAlarmWarningState) {
    // TAKE ACTIONS FOR WARNING ON MASTER HERE
  }
}
