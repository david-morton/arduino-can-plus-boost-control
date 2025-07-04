#include <Arduino.h>

#include "../../shared/debug_logging.h"
#include "../../shared/variables_vehicle_parameters.h"

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Calculate the RPM multiplier based on the current engine speed. This can be thought of as a conversion factor
// which is needed as part of the CAN payload, plus adjustment for an accurate RPM reading in the cluster.
float calculateRpmMultiplier() {
  float              rpmHexConversionMultipler;
  const int          numPoints                                       = 8;
  static const int   measuredRpmValues[numPoints]                    = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000};
  static const float measuredHexConversionMultiplerValues[numPoints] = {5.9, 5.4, 5.2, 5.15, 5.1, 5.05, 5.0, 5.0};

  // Calculate the rpmHexConversionMultipler based on linear interpolation of measured data points
  if (currentEngineSpeedRpm <= 1200) {
    return measuredHexConversionMultiplerValues[0]; // 5.9
  } else if (currentEngineSpeedRpm >= 7000) {
    return measuredHexConversionMultiplerValues[numPoints - 1]; // 5.0
  } else {
    // Find the two nearest RPM values in the table
    int i;
    for (i = 0; i < numPoints - 1; ++i) {
      if (measuredRpmValues[i + 1] >= currentEngineSpeedRpm) {
        break;
      }
    }
    // Perform linear interpolation
    float x0 = measuredRpmValues[i];
    float x1 = measuredRpmValues[i + 1];
    float y0 = measuredHexConversionMultiplerValues[i];
    float y1 = measuredHexConversionMultiplerValues[i + 1];

    rpmHexConversionMultipler = y0 + (y1 - y0) * (currentEngineSpeedRpm - x0) / (x1 - x0);
  }
  return rpmHexConversionMultipler;
};
