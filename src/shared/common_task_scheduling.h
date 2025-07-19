#pragma once

/* ======================================================================
   DEFINES (AND INCLUDE GUARD)
   ====================================================================== */

// Command ID is PAYLOAD_ID_MED_FREQUENCY_MESSAGES and is used on both send and receive sides to identify message type
// This is used so we can handle the messages within this class, as they can only belong to one.

#ifdef ARDUINO_MASTER
#define TELEMETRY_CLASS_LOW MC_MASTER_LOW_FREQUENCY   // Used for sending staged telemetry from master
#define TELEMETRY_CLASS_MED MC_MASTER_MED_FREQUENCY   // Used for sending staged telemetry from master
#define TELEMETRY_CLASS_HIGH MC_MASTER_HIGH_FREQUENCY // Used for sending staged telemetry from master
#else
#define TELEMETRY_CLASS_LOW MC_SLAVE_LOW_FREQUENCY   // Used for sending staged telemetry from slave
#define TELEMETRY_CLASS_MED MC_SLAVE_MED_FREQUENCY   // Used for sending staged telemetry from slave
#define TELEMETRY_CLASS_HIGH MC_SLAVE_HIGH_FREQUENCY // Used for sending staged telemetry from slave
#endif

// #define TELEMETRY_CMD_LOW PAYLOAD_ID_LOW_FREQUENCY_MESSAGES   // Not used
// #define TELEMETRY_CMD_MED PAYLOAD_ID_MED_FREQUENCY_MESSAGES   // Not used
// #define TELEMETRY_CMD_HIGH PAYLOAD_ID_HIGH_FREQUENCY_MESSAGES // Not used

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern unsigned long arduinoLoopExecutionCount;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void handleCommonScheduledTasks();
