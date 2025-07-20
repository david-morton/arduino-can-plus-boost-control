#include <ptScheduler.h>

#include "../../shared/alarm/alarm_helpers.h"
#include "../../shared/debug_logging.h"
#include "../../shared/variables_vehicle_parameters.h"
#include "../check_light/check_light.h"
#include "boost_helpers_master.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

int recommendedBoostTargetGaugeKpa = 0;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Update the boost target Kpa based on various conditions
void updateRecommendedBoostTargetGaugeKpa() {
  static int previousRecommendedBoostTargetGaugeKpa = -1;

  // Set to 0 and return for known states where boost should not be applied
  // TODO: Add in other alarm conditions when sensors are implemented like currentOilTempCelsius < 60 || currentOilPressureGaugeKpa < 100
  if (globalAlarmCriticalState || globalAlarmWarningState || currentCheckEngineLightState != CHECK_LIGHT_OFF ||
      currentSwitchStateClutchEngaged || currentSwitchStateInNeutral || currentEngineTempCelcius < 80) {
    recommendedBoostTargetGaugeKpa = 0;
    // Log the recommended boost target if it has changed
    if (debugBoost && recommendedBoostTargetGaugeKpa != previousRecommendedBoostTargetGaugeKpa) {
      DEBUG_BOOST("Setting recommended boost target to %d kPa", recommendedBoostTargetGaugeKpa);
      previousRecommendedBoostTargetGaugeKpa = recommendedBoostTargetGaugeKpa;
    }
    return;
  }
  // TODO: Implement logic to calculate the boost target based on other parameters like gear, wheel spin etc
  if (currentVehicleSpeedRearKph > 80) {
    recommendedBoostTargetGaugeKpa = 30;
  } else {
    recommendedBoostTargetGaugeKpa = 10;
  }
  // Log the recommended boost target if it has changed
  if (debugBoost && recommendedBoostTargetGaugeKpa != previousRecommendedBoostTargetGaugeKpa) {
    DEBUG_BOOST("Setting recommended boost target to %d kPa", recommendedBoostTargetGaugeKpa);
    previousRecommendedBoostTargetGaugeKpa = recommendedBoostTargetGaugeKpa;
  }
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
