#pragma once

#include <Arduino.h>

/* ======================================================================
   ENUMS
   ====================================================================== */

// This enum defines the telemetry fields we can use across message classes.
// Each can only be mapped into one messaege class, and the mapping is below
typedef enum {
  SENSOR_LUX = 0,
  SENSOR_TEMP,
  NUM_TELEMETRY_FIELDS // Always last
} TelemetryField;

// This enum defines the classes of telemetry messages that can be staged and sent
typedef enum {
  MSG_LOW_FREQUENCY = 0,
  MSG_HIGH_FREQUENCY,
  MSG_SENSOR_EVENT,
  NUM_MESSAGE_CLASSES // Always last
} TelemetryMessageClass;

/* ======================================================================
   STATIC CONSTANTS
   ====================================================================== */
#define MAX_CHARS_PER_KV_PAIR 12 // e.g., "12=-123.45,"
#define SAFETY_MARGIN_BYTES 8    // Some slack for formatting
#define TELEMETRY_MESSAGE_BUFFER_SIZE ((NUM_TELEMETRY_FIELDS * MAX_CHARS_PER_KV_PAIR) + SAFETY_MARGIN_BYTES)

// Ensure the telemetry message buffer size is reasonable for our use case
static_assert(TELEMETRY_MESSAGE_BUFFER_SIZE <= 256, "Telemetry buffer too large!");

/* ======================================================================
   MAPPING TELEMETRY KEYS TO MESSAGE CLASSES
   ====================================================================== */
static inline const TelemetryMessageClass keyToMessageClass[NUM_TELEMETRY_FIELDS] = {
    [SENSOR_LUX]  = MSG_LOW_FREQUENCY,
    [SENSOR_TEMP] = MSG_LOW_FREQUENCY};

/* ======================================================================
    FUNCTION PROTOTYPES
   ====================================================================== */

void addTelemetryItem(TelemetryField key, float value);
void buildAndSendStagedTelemetry(TelemetryMessageClass msgClass, int commandId);
