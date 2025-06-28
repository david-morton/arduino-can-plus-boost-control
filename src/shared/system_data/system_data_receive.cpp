#include <Arduino.h>

#include "../alarm/alarm_helpers.h"
#include "../debug_logging.h"

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Handle incoming system data messages from the remote Arduino
void parseSystemDataPayload(const char *payload) {
  char buffer[64];
  strncpy(buffer, payload, sizeof(buffer)); // Copy payload safely
  buffer[sizeof(buffer) - 1] = '\0';        // Ensure null-termination

  DEBUG_ETHERNET_MESSAGES("Received and parsing system data payload: %s", buffer);

  char *token = strtok(buffer, ",");
  while (token != nullptr) {
    char *equalsSign = strchr(token, '=');
    if (!equalsSign) {
      token = strtok(nullptr, ",");
      continue; // Skip malformed token
    }

    *equalsSign       = '\0'; // Split into key and value
    const char *key   = token;
    const char *value = equalsSign + 1;

    if (*value == '\0') {
      DEBUG_ERROR("Missing value for key: %s while parsing system data payload", key);
    } else if (strcmp(key, "warn") == 0) {
      remoteAlarmWarningState = atoi(value);
    } else if (strcmp(key, "crit") == 0) {
      remoteAlarmCriticalState = atoi(value);
    } else {
      DEBUG_ERROR("Unknown system data key: %s while parsing system data payload", key);
    }

    token = strtok(nullptr, ",");
  }
}
