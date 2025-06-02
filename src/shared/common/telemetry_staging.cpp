#include "telemetry_staging.h"
#include "../common/ethernet/ethernet_send.h"
#include "../common/helpers_logging.h"

/* ======================================================================
   STRUCTURES
   ====================================================================== */
// Create custom struct to hold telemetry data
typedef struct {
  float value;
  bool  valid;
} TelemetrySlot;

/* ======================================================================
   VARIABLES
   ====================================================================== */
// Define the number of telemetry fields
static TelemetrySlot telemetryStaging[NUM_TELEMETRY_FIELDS];

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */
// Add a telemetry item to the staging area
void addTelemetryItem(TelemetryField key, float value) {
  if (key >= NUM_TELEMETRY_FIELDS) {
    DEBUG_ERROR("Invalid telemetry key during staging: %d", key);
    return;
  }
  telemetryStaging[key].value = value;
  telemetryStaging[key].valid = true;
  DEBUG_TELEMETRY("Staged telemetry: key=%d, value=%.2f", key, value);
}

// Build and send staged telemetry for a specific message class
// This function will send all valid telemetry items for the specified message class
void buildAndSendStagedTelemetry(TelemetryMessageClass msgClass, int commandId) {
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
                       "%s%d=%.2f",
                       (offset == 0 ? "" : ","),
                       key,
                       telemetryStaging[key].value);
    offset += len;
  }

  if (offset > 0) {
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
