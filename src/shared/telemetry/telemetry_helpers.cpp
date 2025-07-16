#include "../debug_logging.h"
#include "telemetry_send_staging.h"

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Reverse lookup: get field enum from key string
const char *getTelemetryKeyForField(TelemetryField field) {
  switch (field) {
    case SENSOR_LUX:
      return "lux";
    case SENSOR_CLUTCH:
      return "clutchEngaged";
    case SENSOR_NEUTRAL:
      return "inNeutral";
    case SENSOR_ELECTRONICS_TEMP:
      return "electronicsTemp";
    case SENSOR_RPM:
      return "rpm";
    case BOOST_TARGET:
      return "boostTargetKpa";
    case BOOST_MANIFOLD:
      return "boostManifoldKpa";
    case BOOST_BANK1:
      return "boostBank1Kpa";
    case BOOST_BANK2:
      return "boostBank2Kpa";
    default:
      return nullptr;
  }
}

// Reverse lookup table matching key strings to enum fields
TelemetryField getTelemetryFieldForKey(const char *key) {
  for (int i = 0; i < NUM_TELEMETRY_FIELDS; i++) {
    const char *knownKey = getTelemetryKeyForField((TelemetryField)i);
    if (knownKey != nullptr && strcmp(knownKey, key) == 0) {
      return (TelemetryField)i;
    }
  }
  return NUM_TELEMETRY_FIELDS; // Invalid
}
