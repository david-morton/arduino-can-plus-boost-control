#pragma once

#include <Arduino.h>

/* ======================================================================
   ENUMS
   ====================================================================== */

// This enum defines the telemetry fields we can use across message classes.
// Each can only be mapped into one messaege class, and the mapping is below
typedef enum {
  SENSOR_LUX = 0,
  SENSOR_ELECTRONICS_TEMP,
  NUM_TELEMETRY_FIELDS // Always last, this is used to count the number of fields
} TelemetryField;

// This enum defines the classes of telemetry messages that can be staged and sent
typedef enum {
  MSG_SLAVE_LOW_FREQUENCY = 0,
  MSG_MASTER_LOW_FREQUENCY,
  NUM_MESSAGE_CLASSES // Always last, this is used to count the number of fields
} TelemetryMessageClass;

/* ======================================================================
   STRUCTS
   ====================================================================== */
// This struct is used to stage telemetry data before sending it.
typedef struct {
  float value;
  bool  valid;
} TelemetrySlot;

/* ======================================================================
   STATIC CONSTANTS
   ====================================================================== */
#define MAX_CHARS_PER_KV_PAIR 20 // e.g., "engineTemp=-123.45,"
#define SAFETY_MARGIN_BYTES 8    // Some slack for formatting
#define TELEMETRY_MESSAGE_BUFFER_SIZE ((NUM_TELEMETRY_FIELDS * MAX_CHARS_PER_KV_PAIR) + SAFETY_MARGIN_BYTES)

// Ensure the telemetry message buffer size is reasonable for our use case.
// This may need to be adjusted based on the actual number of fields and their expected sizes.
static_assert(TELEMETRY_MESSAGE_BUFFER_SIZE <= 256, "Telemetry buffer too large!");

/* ======================================================================
   VARIABLES
   ====================================================================== */
extern TelemetrySlot telemetryStaging[NUM_TELEMETRY_FIELDS];

/* ======================================================================
   MAPPING TELEMETRY KEYS TO MESSAGE CLASSES
   ====================================================================== */
static inline const TelemetryMessageClass keyToMessageClass[NUM_TELEMETRY_FIELDS] = {
    [SENSOR_LUX]              = MSG_SLAVE_LOW_FREQUENCY,
    [SENSOR_ELECTRONICS_TEMP] = MSG_SLAVE_LOW_FREQUENCY};

/* ======================================================================
    FUNCTION PROTOTYPES
   ====================================================================== */

void           buildTelemetryItem(TelemetryField key, float value);
void           sendStagedTelemetry(TelemetryMessageClass msgClass, int commandId);
TelemetryField getTelemetryFieldForKey(const char *key);
const char    *getTelemetryKeyForField(TelemetryField field);
