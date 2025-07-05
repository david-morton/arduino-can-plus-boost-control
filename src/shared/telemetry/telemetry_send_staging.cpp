#include "telemetry_send_staging.h"
#include "../debug_logging.h"
#include "../ethernet/ethernet_send_udp.h"
#include "telemetry_helpers.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

// Define the number of telemetry fields
TelemetrySlot telemetryStaging[NUM_TELEMETRY_FIELDS];

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Add a telemetry item to the staging area ready to pickup and send
// New sensor key value pais are defined in getTelemetryKeyForField and telemetry_send_staging.h
void buildTelemetryItem(TelemetryField key, float value) {
  if (key >= NUM_TELEMETRY_FIELDS) {
    DEBUG_ERROR("Invalid telemetry key during staging: %d", key);
    return;
  }

  telemetryStaging[key].value = value;
  telemetryStaging[key].valid = true;

  // DEBUG_TELEMETRY("Staged telemetry: key=%d (%s), value=%.2f", key, getTelemetryKeyForField((TelemetryField)key), value);
}

// Build and send staged telemetry for a specific message class
// This function will send all valid telemetry items for the specified message class
void sendStagedTelemetry(TelemetryMessageClass msgClass, int commandId) {
  if (msgClass >= NUM_MESSAGE_CLASSES) {
    DEBUG_ERROR("Invalid message class: %d", msgClass);
    return;
  }

  char   messageBuffer[TELEMETRY_MESSAGE_BUFFER_SIZE];
  size_t offset = 0;

  for (int key = 0; key < NUM_TELEMETRY_FIELDS; key++) {
    if (keyToMessageClass[key] != msgClass) {
      continue; // Skip if the key does not match the requested message class
    }
    if (!telemetryStaging[key].valid) {
      continue; // Skip if the telemetry item is not valid
    }
    if (telemetryStaging[key].value == telemetryStaging[key].lastSent) {
      continue; // Skip if the value hasn't changed since last sent
    }

    int len = snprintf(messageBuffer + offset,
                       sizeof(messageBuffer) - offset,
                       "%s%s=%g",
                       (offset == 0 ? "" : ","),
                       getTelemetryKeyForField((TelemetryField)key),
                       telemetryStaging[key].value);
    offset += len;
  }

  if (offset > 0) {
    if (offset >= TELEMETRY_MESSAGE_BUFFER_SIZE - 1) {
      DEBUG_ERROR("Telemetry message exceeds buffer size! Size: %d / Max: %d", offset, TELEMETRY_MESSAGE_BUFFER_SIZE);
      return;
    }

    sendUdpMessageWithCommand(commandId, messageBuffer);
    DEBUG_TELEMETRY("Submitted buffer to be sent via UDP: %s", messageBuffer);
  }

  // Clear and update sent values only for relevant fields
  for (int key = 0; key < NUM_TELEMETRY_FIELDS; key++) {
    if (keyToMessageClass[key] == msgClass && telemetryStaging[key].valid) {
      telemetryStaging[key].lastSent = telemetryStaging[key].value;
      telemetryStaging[key].valid    = false;
    }
  }
}
