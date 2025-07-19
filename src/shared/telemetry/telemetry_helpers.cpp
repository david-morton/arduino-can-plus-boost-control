#include "../debug_logging.h"
#include "telemetry_send_staging.h"

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Reverse lookup: get field enum from key string
const char *getTelemetryKeyForField(TelemetryField field) {
  switch (field) {
    case TM_LUX:
      return "lux";
    case TM_CLUTCH_IN:
      return "clutchEngaged";
    case TM_IN_NEUTRAL:
      return "inNeutral";
    case TM_ELECTRONICS_TEMP:
      return "electronicsTemp";
    case TM_RPM:
      return "rpm";
    case TM_BOOST_RECOMMENDATION:
      return "boostRecommendationKpa";
    case TM_BOOST_MANIFOLD:
      return "boostManifoldKpa";
    case TM_BOOST_BANK1:
      return "boostBank1Kpa";
    case TM_BOOST_BANK2:
      return "boostBank2Kpa";
    default:
      return nullptr;
  }
}

// Reverse lookup table matching key strings to enum fields
TelemetryField getTelemetryFieldForKey(const char *key) {
  for (int i = 0; i < TM_COUNT; i++) {
    const char *knownKey = getTelemetryKeyForField((TelemetryField)i);
    if (knownKey != nullptr && strcmp(knownKey, key) == 0) {
      return (TelemetryField)i;
    }
  }
  return TM_COUNT; // Invalid
}
