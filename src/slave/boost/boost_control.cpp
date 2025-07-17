#include <ptScheduler.h>

#include "../../shared/debug_logging.h"
#include "../../shared/variables_vehicle_parameters.h"
#include "boost_control.h"
#include "boost_helpers_slave.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

int currentIntakePressureBank1GaugeKpa    = 0;
int currentIntakePressureBank2GaugeKpa    = 0;
int currentIntakePressureManifoldGaugeKpa = 0;

/* ======================================================================
   SCHEDULER OBJECTS
   ====================================================================== */

ptScheduler ptReadIntakePressuresGaugeKpa(PT_TIME_100MS);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// TODO: We want to initialise our referance pressures before startupup to account for altitude or similar

void handleBoosControlTasks() {

  // Read intake pressure sensors
  if (ptReadIntakePressuresGaugeKpa.call()) {
    currentIntakePressureBank1GaugeKpa    = readBosch3BarTmapPressure(PIN_TMAP_PRESSURE_BANK1);
    currentIntakePressureBank2GaugeKpa    = readBosch3BarTmapPressure(PIN_TMAP_PRESSURE_BANK2);
    currentIntakePressureManifoldGaugeKpa = readBosch3BarTmapPressure(PIN_TMAP_PRESSURE_MANIFOLD);

    // Log the pressure readings
    DEBUG_BOOST("Intake Pressure Bank 1: %d kPa", currentIntakePressureBank1GaugeKpa);
    DEBUG_BOOST("Intake Pressure Bank 2: %d kPa", currentIntakePressureBank2GaugeKpa);
    DEBUG_BOOST("Intake Pressure Manifold: %d kPa", currentIntakePressureManifoldGaugeKpa);
  }
}