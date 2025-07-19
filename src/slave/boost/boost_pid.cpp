#include <PID_v1.h>

#include "../../shared/debug_logging.h"
#include "../../shared/variables_vehicle_parameters.h"
#include "../pin_assignments_slave.h"
#include "boost_control.h"

/* ======================================================================
   VAIABLES
   ====================================================================== */

// PID controllers for each bank and manifold pressure
double kpBank1, kiBank1, kdBank1;
double kpBank2, kiBank2, kdBank2;

/* ======================================================================
   PID CONTROL OBJECTS
   ====================================================================== */

// PID pidBank1(&currentIntakePressureBank1GaugeKpa, &boostSolenoidPwmBank1, &boostTargetGaugeKpa, kpBank1, kiBank1, kdBank1, DIRECT);
// PID pidBank2(&currentIntakePressureBank2GaugeKpa, &boostSolenoidPwmBank2, &boostTargetGaugeKpa, kpBank2, kiBank2, kdBank2, DIRECT);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Configure the PID controllers for boost control
// void configureBoostControlPidControl() {
//   pidBank1.SetMode(AUTOMATIC);
//   pidBank2.SetMode(AUTOMATIC);

//   pidBank1.SetOutputLimits(0, 255);
//   pidBank2.SetOutputLimits(0, 255);

//   pidBank1.SetSampleTime(50); // 20Hz
//   pidBank2.SetSampleTime(50);
// }
