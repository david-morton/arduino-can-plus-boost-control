#include <Arduino.h>

#include "../alarm/alarm_helpers.h"
#include "../debug_logging.h"

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Handle incoming system data messages from the remote Arduino
void parseSystemDataPayload(const char *payload) {
  char buffer[64];
  strncpy(buffer, payload, sizeof(buffer));
  buffer[sizeof(buffer) - 1] = '\0';

  char *token = strtok(buffer, ",");
  while (token != nullptr) {
    char *equalsSign = strchr(token, '=');
    *equalsSign      = '\0';

    const char *key   = token;
    const char *value = equalsSign + 1;

    if (value == nullptr || *value == '\0') {
      DEBUG_ERROR("Missing value for key: %s while parsing system data payload", key);
      token = strtok(nullptr, ",");
      continue;
    }

    if (strcmp(key, "warn") == 0) {
      remoteAlarmWarningState = atoi(value);
    } else if (strcmp(key, "crit") == 0) {
      remoteAlarmCriticalState = atoi(value);
    } else {
      DEBUG_ERROR("Unknown system data key: %s while parsing system data payload", key);
    }

    token = strtok(nullptr, ",");
  }
}
