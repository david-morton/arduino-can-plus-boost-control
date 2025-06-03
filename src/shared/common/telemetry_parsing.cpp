#include <Arduino.h>

#include "helpers_logging.h"
#include "telemetry_parsing.h"
#include "telemetry_staging.h"

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// This helper function retrieves the telemetry field for a given key if it is marked as valid
bool consumeTelemetryFloat(TelemetryField field, float* outValue) {
  if (!telemetryStaging[field].valid)
    return false;

  *outValue = telemetryStaging[field].value;
  telemetryStaging[field].valid = false;
  return true;
}

// This function parses a telemetry payload string and updates the telemetry staging area
// Once values are in the staging buffer, they can be read by other parts of the code
void parseTelemetryPayload(const char *payload) {
  // Define a buffer size for the telemetry message
  char buffer[TELEMETRY_MESSAGE_BUFFER_SIZE];

  // Check if payload i too large to fit in the buffer
  if (strlen(payload) >= TELEMETRY_MESSAGE_BUFFER_SIZE) {
    DEBUG_ERROR("Telemetry message too long (%d bytes), max is %d", strlen(payload), TELEMETRY_MESSAGE_BUFFER_SIZE - 1);
    return;
  }

  // Copy the payload into the buffer, ensuring it is null-terminated
  strncpy(buffer, payload, sizeof(buffer));
  buffer[sizeof(buffer) - 1] = '\0';

  char *pair = strtok(buffer, ",");
  while (pair != nullptr) {
    char *equals = strchr(pair, '=');
    if (!equals) {
      DEBUG_ERROR("Telemetry parse error: missing '=' in pair [%s]", pair);
      pair = strtok(nullptr, ",");
      continue;
    }

    *equals              = '\0';
    const char *key      = pair;
    const char *valueStr = equals + 1;

    TelemetryField field = getTelemetryFieldForKey(key);
    if (field == NUM_TELEMETRY_FIELDS) {
      DEBUG_ERROR("Telemetry parse error: unknown key [%s]", key);
      pair = strtok(nullptr, ",");
      continue;
    }

    float value = atof(valueStr);
    if (value == 0 && valueStr[0] != '0') {
      DEBUG_ERROR("Telemetry parse error: invalid float value for key [%s]: [%s]", key, valueStr);
      pair = strtok(nullptr, ",");
      continue;
    }

    telemetryStaging[field].value = value;
    telemetryStaging[field].valid = true;

    DEBUG_TELEMETRY("Received telemetry [%s] = %.2f", key, value);

    pair = strtok(nullptr, ",");
  }
}
