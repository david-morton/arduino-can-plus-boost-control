#include <Arduino.h>

#include "../../shared/alarm/alarm_helpers.h"
#include "../../shared/debug_logging.h"
#include "../../shared/ethernet/ethernet_helpers.h"
#include "../../shared/variables_programmatic.h"
#include "../../shared/variables_vehicle_parameters.h"
#include "alarms_master.h"

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Check all current critical alarm conditions, and update the alarm state accordingly
void updateLocalCriticalAlarmState() {
  // TODO: Add more critical alarm conditions as needed
  if (currentEngineTempCelcius < alarmThresholdsCritical.engineTemperature) {
    localAlarmCriticalState = true;
  } else {
    localAlarmCriticalState = false;
  }
}

// Check all current warning alarm conditions, and update the alarm state accordingly
void updateLocalWarningAlarmState() {
  // TODO: Add more warning alarm conditions as needed
  if (pingLossPercent > alarmThresholdsWarning.pingLossPercent || !globalHealthEthernetConnected || !globalHealthEthernetPeerOnline) {
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
    // TODO: Evaluate if there are any actions to perform here or if they are handled in respective logic
  } else if (globalAlarmWarningState) {
    // TODO: Evaluate if there are any actions to perform here or if they are handled in respective logic
  }
}
