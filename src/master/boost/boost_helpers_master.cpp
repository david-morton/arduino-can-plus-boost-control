#include <ptScheduler.h>

#include "../../shared/alarm/alarm_helpers.h"
#include "../../shared/debug_logging.h"
#include "../../shared/variables_vehicle_parameters.h"
#include "../check_light/check_light.h"
#include "boost_helpers_master.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

int currentBoostTargetGaugeKpa = 0;

/* ======================================================================
   SCHEDULER OBJECTS
   ====================================================================== */

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Update the boost target Kpa based on various conditions
void updateBoostTargetGaugeKpa() {
  // Set to 0 and return for known states where boost should not be applied
  if (globalAlarmCriticalState || globalAlarmWarningState || currentCheckEngineLightState != CHECK_LIGHT_OFF ||
      currentSwitchStateClutchEngaged || currentSwitchStateInNeutral ||
      currentEngineTempCelcius < 80 || currentOilTempCelsius < 60 || currentOilPressureGaugeKpa < 100) {
    currentBoostTargetGaugeKpa = 0;
    return;
  }
  currentBoostTargetGaugeKpa = 20;
  // TODO: Implement logic to calculate the boost target based on other parameters like gear, wheel spin etc
}

/* ======================================================================
   INFO: A simple lookup table of kPa to psi
   ======================================================================
 +--------+--------+
 |  kPa   |  psi   |
 +--------+--------+
 |   0    |  0.00  |
 |  10    |  1.45  |
 |  20    |  2.90  |
 |  30    |  4.35  |
 |  40    |  5.80  |
 |  50    |  7.25  |
 |  60    |  8.70  |
 |  70    | 10.15  |
 |  80    | 11.60  |
 |  90    | 13.05  |
 | 100    | 14.50  |
*/
