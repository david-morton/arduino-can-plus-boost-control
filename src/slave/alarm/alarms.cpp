#include <Arduino.h>

#include "../../shared/debug_logging.h"
#include "../../shared/variables_programmatic.h"
#include "../../shared/variables_vehicle_parameters.h"
#include "alarm_helpers.h"
#include "alarms.h"

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Check all current alarm conditions, and update the alarm state accordingly
void checkAndUpdateAlarmState() {
  if (currentAmbientLux < alarmThresholds.ambientLuxMin ||
      currentEngineSpeedRpm > alarmThresholds.engineRpmMax ||
      pingLossPercent > alarmThresholds.pingLossPercent) {
    alarmEnable(currentEngineSpeedRpm);
  } else {
    alarmDisable();
  }
}

// TODO: Add warning buzzer blip for ping loss say
