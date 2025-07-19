#include <ptScheduler.h>

#include "../../shared/alarm/alarm_helpers.h"
#include "../../shared/debug_logging.h"
#include "../../shared/variables_vehicle_parameters.h"
#include "../pin_assignments_slave.h"
#include "boost_control.h"
#include "boost_helpers_slave.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

bool targetZeroBoost                       = false; // Global failsafe flag to set target boost to zero
int  boostTargetGaugeKpa                   = 0;
int  currentIntakePressureBank1GaugeKpa    = 0;
int  currentIntakePressureBank2GaugeKpa    = 0;
int  currentIntakePressureManifoldGaugeKpa = 0;

/* ======================================================================
   SCHEDULER OBJECTS
   ====================================================================== */

// Medium frequency tasks (hundreds of milliseconds)
ptScheduler ptCheckGlobalWarningAndErrorConditions(PT_TIME_50MS);
ptScheduler ptReadIntakePressuresGaugeKpa(PT_TIME_100MS);
ptScheduler ptUpdateBoostTargetGaugeKpa(PT_TIME_100MS);

// Low frequency tasks (seconds)
ptScheduler ptOutputBoostDebug(PT_TIME_5S);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// TODO: We want to initialise our referance pressures before startupup to account for altitude or similar

void handleBoosControlTasks() {
  // Check for global warning and error conditions and set target boost to zero if any are present. We will use this to
  // override any target value which may be set by the master Arduino if comms are lost or a local alarm condition is triggered.
  if (ptCheckGlobalWarningAndErrorConditions.call()) {
    if (globalAlarmWarningState || globalAlarmCriticalState) {
      targetZeroBoost = true;
    } else {
      targetZeroBoost = false;
    }
  }

  // Update the target boost value from the master Arduino if it is not zero and no global alarm conditions are present
  if (ptUpdateBoostTargetGaugeKpa.call()) {
    if (targetZeroBoost) {
      boostTargetGaugeKpa = 0;
    } else {
      boostTargetGaugeKpa = recommendedBoostTargetGaugeKpa;
    }
  }

  // Read intake pressure sensors
  if (ptReadIntakePressuresGaugeKpa.call()) {
    currentIntakePressureBank1GaugeKpa    = readBosch3BarTmapPressure(PIN_TMAP_PRESSURE_BANK1);
    currentIntakePressureBank2GaugeKpa    = readBosch3BarTmapPressure(PIN_TMAP_PRESSURE_BANK2);
    currentIntakePressureManifoldGaugeKpa = readBosch3BarTmapPressure(PIN_TMAP_PRESSURE_MANIFOLD);
  }

  // TODO: Implement boost solenoid control logic here based on the target boost and current intake pressures.

  // Log debug data when needed
  if (debugBoost && ptOutputBoostDebug.call()) {
    if (targetZeroBoost) {
      DEBUG_ERROR("Global warning or error condition detected, setting boost target to zero.");
    }
    DEBUG_BOOST("Intake Pressures are: Bank 1: %d kPa, Bank 2: %d kPa, Manifold: %d kPa against a target of %d kPa",
                currentIntakePressureBank1GaugeKpa,
                currentIntakePressureBank2GaugeKpa,
                currentIntakePressureManifoldGaugeKpa,
                boostTargetGaugeKpa);
  }
}
