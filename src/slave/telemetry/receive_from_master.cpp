#include <ptScheduler.h>

#include "../../shared/debug_logging.h"
#include "../../shared/telemetry/telemetry_receive_parser.h"
#include "../../shared/variables_vehicle_parameters.h"

/* ======================================================================
   VARIABLES: From remote Arduino
   ====================================================================== */

int recommendedBoostTargetGaugeKpa = 0;

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */

float valueFromRemote; // Scratch variable to transport variable values from remote Arduino

/* ======================================================================
   SCHEDULER OBJECTS
   ====================================================================== */

// Medium frequency tasks (hundreds of milliseconds)
ptScheduler ptGetRecommendedBoostTargetGaugeKpa = ptScheduler(PT_TIME_100MS);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Handle staged telemetry data received from remote Arduino (master)
void handleTelemetryReceivedFromMaster() {

  // Get the recommended boost target gauge Kpa from the master Arduino via staged data
  if (ptGetRecommendedBoostTargetGaugeKpa.call() && handleTelemetryFloat(TM_BOOST_RECOMMENDATION, &valueFromRemote)) {
    recommendedBoostTargetGaugeKpa = valueFromRemote;
  }
}
