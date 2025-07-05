#include "telemetry_send_staging.h"
#include "../debug_logging.h"

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Reverse lookup: get field enum from key string
const char *getTelemetryKeyForField(TelemetryField field) {
  switch (field) {
    case SENSOR_LUX:
      return "lux";
    case SENSOR_CLUTCH:
      return "clutch";
    case SENSOR_NEUTRAL:
      return "neutral";
    case SENSOR_ELECTRONICS_TEMP:
      return "electronicsTemp";
    case SENSOR_RPM:
      return "rpm";
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
