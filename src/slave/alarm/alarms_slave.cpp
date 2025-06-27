#include <Arduino.h>

#include "../../shared/alarm/alarm_helpers.h"
#include "../../shared/debug_logging.h"
#include "../../shared/variables_programmatic.h"
#include "../../shared/variables_vehicle_parameters.h"
#include "alarm_buzzer.h"
#include "alarms_slave.h"

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Check all current critical alarm conditions, and update the alarm state accordingly
void updateLocalCriticalAlarmState() {
  if (currentAmbientLux < alarmThresholdsCritical.ambientLuxMin ||
      currentEngineSpeedRpm > alarmThresholdsCritical.engineRpmMax) {
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
void handleGlobalAlarmStates() {
  if (currentEngineSpeedRpm < 100) {
    if (alarmBuzzerIsSounding) {
      alarmBuzzerDisable();
    }
    return; // Do not process alarms if the engine is not running
  }

  updateLocalCriticalAlarmState();
  updateLocalWarningAlarmState();
  evaluateGlobalAlarmStates();

  // If the global critical alarm state is active, enable the buzzer
  if (globalAlarmCriticalState) {
    alarmBuzzerEnable();
  } else if (globalAlarmWarningState) {
    // TAKE ACTIONS FOR WARNING ON SLAVE HERE
  } else {
    // If no alarms are active, disable the buzzer
    alarmBuzzerDisable();
  }
}
