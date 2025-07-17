#include "boost_control.h"
#include "../../shared/debug_logging.h"
#include "../../shared/variables_vehicle_parameters.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

int currentBoostTargetGaugeKpa            = 0;
int currentIntakePressureBank1GaugeKpa    = 0;
int currentIntakePressureBank2GaugeKpa    = 0;
int currentIntakePressureManifoldGaugeKpa = 0;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// TODO: We want to initialise our referance pressures before startupup to account for altitude or similar
