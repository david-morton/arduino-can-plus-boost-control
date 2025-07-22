#include <PID_v1.h>

#include "../../shared/debug_logging.h"
#include "../pin_assignments_slave.h"
#include "boost_control.h"

/* ======================================================================
   VAIABLES
   ====================================================================== */

// PID variables for each bank of the boost control system
double kpBank1 = 1.2, kiBank1 = 0.01, kdBank1 = 0.1;
double kpBank2 = 1.2, kiBank2 = 0.01, kdBank2 = 0.1;

double boostSolenoidPwmBank1 = 0.0;
double boostSolenoidPwmBank2 = 0.0;

/* ======================================================================
   PID CONTROL OBJECTS
   ====================================================================== */

PID pidBank1(&currentIntakePressureBank1GaugeKpa, &boostSolenoidPwmBank1, &boostTargetGaugeKpa, kpBank1, kiBank1, kdBank1, DIRECT);
PID pidBank2(&currentIntakePressureBank2GaugeKpa, &boostSolenoidPwmBank2, &boostTargetGaugeKpa, kpBank2, kiBank2, kdBank2, DIRECT);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Configure the PID controllers for boost control
void configureBoostControlPidControl() {
  pidBank1.SetMode(AUTOMATIC);
  pidBank2.SetMode(AUTOMATIC);

  pidBank1.SetOutputLimits(0, 255);
  pidBank2.SetOutputLimits(0, 255);

  // These are set artificially low as we are calling Compute() via scheduler
  pidBank1.SetSampleTime(5);
  pidBank2.SetSampleTime(5);
}

// Calculate and apply the PID control for boost solenoids
void calculateAndApplyBoostControl() {
  pidBank1.Compute();
  pidBank2.Compute();

  if (boostTargetGaugeKpa == 0) {
    // If target boost is zero, set PWM to zero for both banks
    // TODO: Evaluate if this is around the right way or if it should be 255
    analogWrite(PIN_BOOST_SOLENOID_BANK1, 0);
    analogWrite(PIN_BOOST_SOLENOID_BANK2, 0);
  } else {
    // Apply the calculated PWM values to the boost solenoids
    analogWrite(PIN_BOOST_SOLENOID_BANK1, boostSolenoidPwmBank1);
    analogWrite(PIN_BOOST_SOLENOID_BANK2, boostSolenoidPwmBank2);
  }
}
