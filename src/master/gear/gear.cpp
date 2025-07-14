#include "gear.h"
#include "../../shared/debug_logging.h"
#include "../../shared/variables_vehicle_parameters.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

int   currentGear             = 0;
float tyreCircumferenceMetres = calculateTyreCircumference();

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Calculate the tyre circumference based on the header-defined tyre size
float calculateTyreCircumference() {
  float sidewallHeightMm = TYRE_WIDTH_MM * TYRE_ASPECT_RATIO;
  float wheelDiameterMm  = WHEEL_DIAMETER_IN * 25.4;
  float totalDiameterMm  = 2.0f * sidewallHeightMm + wheelDiameterMm;

  float totalDiameterM = totalDiameterMm / 1000.0f;
  float circumferenceM = PI * totalDiameterM;

  return circumferenceM;
}

// Report on the tyre circumference and diameter along with how it was calculated
void reportTyreCircumference() {
  float diameterMetres = tyreCircumferenceMetres / PI;
  DEBUG_GENERAL("Calculating tyre circumference ...");
  DEBUG_GENERAL("\t\tCircumference: %.2fm, Diameter: %.2fm (Width: %dmm, Aspect Ratio: %.2f, Wheel Diameter: %d\")",
                tyreCircumferenceMetres, diameterMetres, (int)TYRE_WIDTH_MM, TYRE_ASPECT_RATIO, (int)WHEEL_DIAMETER_IN);
}

// Update the current gear based on vehicle speed and engine RPM
void updateCurrentGear() {
  static int lastReportedGear = -99; // Invalid initial value to force first debug output

  // Return 0 if in neutral or clutch is disengaged
  if (currentSwitchStateNeutral || currentSwitchStateClutch) {
    currentGear = 0;
    if (debugGears && lastReportedGear != 0) {
      DEBUG_GEARS("Current gear is 0 (neutral or clutch disengaged)");
    }
    lastReportedGear = 0; // Update last reported gear to avoid repeated messages
    return;
  }

  // Ignore if speed or RPM are too low to infer gear (will also dictate no boost target)
  if (currentVehicleSpeedRearKph < 2 || currentEngineSpeedRpm < 500) {
    currentGear = -1; // Indicate unknown gear
    if (debugGears && lastReportedGear != -1) {
      DEBUG_GEARS("Current gear is unknown (speed: %.2f kph, RPM: %d)", currentVehicleSpeedRearKph, currentEngineSpeedRpm);
    }
    lastReportedGear = -1; // Update last reported gear to avoid repeated messages
    return;
  }

  float vehicleSpeedMPerMin = (currentVehicleSpeedRearKph * 1000.0f) / 60.0f;

  float observedRatio = (currentEngineSpeedRpm * tyreCircumferenceMetres) / (vehicleSpeedMPerMin * FINAL_DRIVE_RATIO);

  float smallestError = 9999.0f;
  int   bestGear      = 0;

  for (int i = 0; i < NUM_GEARS; ++i) {
    float error = fabs(observedRatio - GEAR_RATIOS[i]);
    if (error < smallestError) {
      smallestError = error;
      bestGear      = i + 1;
    }
    currentGear = bestGear;
  }

  // Only output debug if gear has changed
  if (debugGears && currentGear != lastReportedGear) {
    DEBUG_GEARS("Observed ratio: %.2f, Best gear: %d, Error: %.4f, Speed: %.2f kph, RPM: %d",
                observedRatio, bestGear, smallestError, currentVehicleSpeedRearKph, currentEngineSpeedRpm);
    lastReportedGear = currentGear;
  }
}
