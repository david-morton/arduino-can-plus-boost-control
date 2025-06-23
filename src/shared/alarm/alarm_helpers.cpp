#include <Arduino.h>

#include "../../shared/debug_logging.h"
#include "alarm_helpers.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

bool globalAlarmCriticalState = false;
bool globalAlarmWarningState  = false;
bool localAlarmCriticalState  = false;
bool localAlarmWarningState   = false;
bool remoteAlarmCriticalState = false;
bool remoteAlarmWarningState  = false;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Send alarm status message to remote Arduino
// void sendAlarmStatusMessage() {
//   DEBUG_GENERAL("Sending alarm status message to remote Arduino ...");

//   // Build the telemetry item with the current alarm states
//   buildTelemetryItem(SENSOR_ALARM_CRITICAL, localAlarmCriticalState);
//   buildTelemetryItem(SENSOR_ALARM_WARNING, localAlarmWarningState);

//   // Send the staged telemetry data
//   sendStagedTelemetry(MSG_SLAVE_ALARM_STATUS, CMD_ALARM_STATUS);
// }

// Evaluate the current alarm states and update global states
void evaluateGlobalAlarmStates() {
  globalAlarmCriticalState = localAlarmCriticalState || remoteAlarmCriticalState;
  globalAlarmWarningState  = localAlarmWarningState || remoteAlarmWarningState;

  DEBUG_ERROR("Global Alarm States: Critical: %s, Warning: %s",
                globalAlarmCriticalState ? "ON" : "OFF",
                globalAlarmWarningState ? "ON" : "OFF");
}
