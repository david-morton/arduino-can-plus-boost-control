#include <Arduino.h>

#include "../../shared/debug_logging.h"
#include "alarm_helpers.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

bool globalAlarmCriticalState = false; // Global critical alarm state, true if any local or remote critical alarm is active
bool globalAlarmWarningState  = false; // Global warning alarm state, true if any local or remote warning alarm is active
bool localAlarmCriticalState  = false; // Local critical alarm state, true if any local critical alarm is active
bool localAlarmWarningState   = false; // Local warning alarm state, true if any local warning alarm is active
bool remoteAlarmCriticalState = false; // Remote critical alarm state, true if any remote critical alarm is active
bool remoteAlarmWarningState  = false; // Remote warning alarm state, true if any remote warning alarm is active

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Evaluate the current alarm states and update global states
void evaluateGlobalAlarmStates() {
  globalAlarmCriticalState = localAlarmCriticalState || remoteAlarmCriticalState;
  globalAlarmWarningState  = localAlarmWarningState || remoteAlarmWarningState;

  if (globalAlarmWarningState || globalAlarmCriticalState) {
    DEBUG_ERROR("Global Alarm States: Critical: %s, Warning: %s",
                globalAlarmCriticalState ? "ON" : "OFF",
                globalAlarmWarningState ? "ON" : "OFF");
  }
}
