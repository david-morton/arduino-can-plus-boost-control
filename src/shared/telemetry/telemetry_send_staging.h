#pragma once

#include <Arduino.h>

/* ======================================================================
   ENUM DEFINITIONS
   ====================================================================== */

// This enum defines the telemetry fields we can use across message classes.
// Each can only be mapped into ONE messaege class, and the mappings are defined below.
// The TM_ prefix is used to denote 'Telemetry Metric'.
typedef enum {
  TM_LUX = 0,
  TM_ELECTRONICS_TEMP,
  TM_CLUTCH_IN,
  TM_IN_NEUTRAL,
  TM_RPM,
  TM_BOOST_BANK1,
  TM_BOOST_BANK2,
  TM_BOOST_MANIFOLD,
  TM_BOOST_RECOMMENDATION,
  TM_COUNT // Always last, this is used to count the number of fields
} TelemetryField;

// This enum defines the classes of telemetry messages that can be staged and sent.
// The Arduino role noted here is used to determine which side of the communication is sending the message.
typedef enum {
  MC_SLAVE_LOW_FREQUENCY = 0,
  MC_SLAVE_MED_FREQUENCY,
  MC_SLAVE_HIGH_FREQUENCY,
  MC_MASTER_LOW_FREQUENCY,
  MC_MASTER_MED_FREQUENCY,
  MC_MASTER_HIGH_FREQUENCY,
  MC_COUNT // Always last, this is used to count the number of fields
} TelemetryMessageClass;

/* ======================================================================
   MAPPING TELEMETRY KEYS TO MESSAGE CLASSES
   ====================================================================== */

// This mapping defines which telemetry fields go into which message classes.
// The frequency for each class is via ptScheduler objects in common_task_scheduling.cpp
// and it is from the perspective of the sender. ie: MSG_SLAVE denotes that the slave
// Arduino is sending this message class.
static inline const TelemetryMessageClass keyToMessageClass[TM_COUNT] = {
    [TM_LUX]                  = MC_SLAVE_LOW_FREQUENCY,
    [TM_ELECTRONICS_TEMP]     = MC_SLAVE_LOW_FREQUENCY,
    [TM_CLUTCH_IN]            = MC_SLAVE_MED_FREQUENCY,
    [TM_IN_NEUTRAL]           = MC_SLAVE_MED_FREQUENCY,
    [TM_RPM]                  = MC_SLAVE_HIGH_FREQUENCY,
    [TM_BOOST_BANK1]          = MC_SLAVE_MED_FREQUENCY,
    [TM_BOOST_BANK2]          = MC_SLAVE_MED_FREQUENCY,
    [TM_BOOST_MANIFOLD]       = MC_SLAVE_MED_FREQUENCY,
    [TM_BOOST_RECOMMENDATION] = MC_MASTER_MED_FREQUENCY};

/* ======================================================================
   STRUCTS
   ====================================================================== */

// This struct is used to stage telemetry data before sending it.
typedef struct {
  float value;    // The actual telemetry value, e.g., temperature, lux, etc.
  float lastSent; // The last value sent, used to deduplicate when sending
  bool  valid;    // True if value is valid (e.g., not NaN)
} TelemetrySlot;

/* ======================================================================
   CONSTANTS
   ====================================================================== */

constexpr int MAX_CHARS_PER_KV_PAIR         = 20; // e.g., "engineTemp=-123.45,"
constexpr int SAFETY_MARGIN_BYTES           = 8;  // Some slack for formatting
constexpr int TELEMETRY_MESSAGE_BUFFER_SIZE = ((TM_COUNT * MAX_CHARS_PER_KV_PAIR) + SAFETY_MARGIN_BYTES);

/* ======================================================================
   ASSERTIONS
   ====================================================================== */

// Ensure the telemetry message buffer size is reasonable for our use case.
// This may need to be adjusted based on the actual number of fields and their expected sizes.
static_assert(TELEMETRY_MESSAGE_BUFFER_SIZE <= 256, "Telemetry buffer too large!");

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern TelemetrySlot telemetryStaging[TM_COUNT];

/* ======================================================================
    FUNCTION PROTOTYPES
   ====================================================================== */

void buildTelemetryItem(TelemetryField key, float value);
void sendStagedTelemetry(TelemetryMessageClass msgClass, int commandId);
