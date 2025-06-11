#include "telemetry_send_staging.h"
#include "../shared/debug_logging.h"
#include "../shared/ethernet/ethernet_send_udp.h"

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
  DEBUG_TELEMETRY("Staged telemetry: key=%d (%s), value=%.2f", key, getTelemetryKeyForField((TelemetryField)key), value);
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
      continue;
    }
    if (telemetryStaging[key].valid == false) {
      continue;
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
    // Ensure the message is not filling our buffer beyond its size
    if (offset >= TELEMETRY_MESSAGE_BUFFER_SIZE - 1) {
      DEBUG_ERROR("Telemetry message exceeds buffer size! Size: %d / Max: %d", offset, TELEMETRY_MESSAGE_BUFFER_SIZE);
      return; // Don't send incomplete or corrupted message
    }

    sendUdpMessageWithCommand(commandId, messageBuffer);
    DEBUG_TELEMETRY("Submitted buffer to be sent via UDP: %s", messageBuffer);
  }

  // Clear only the keys for this message class
  for (int key = 0; key < NUM_TELEMETRY_FIELDS; key++) {
    if (keyToMessageClass[key] == msgClass) {
      telemetryStaging[key].valid = false;
    }
  }
}

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
