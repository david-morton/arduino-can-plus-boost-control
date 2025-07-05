#pragma once

/* ======================================================================
   INCLUDE GUARD
   ====================================================================== */

#ifdef ARDUINO_MASTER
  #define TELEMETRY_CLASS_LOW     MSG_MASTER_LOW_FREQUENCY
  #define TELEMETRY_CLASS_MED     MSG_MASTER_MED_FREQUENCY
  #define TELEMETRY_CLASS_HIGH    MSG_MASTER_HIGH_FREQUENCY
  #define TELEMETRY_CMD_LOW       CMD_LOW_FREQUENCY_MESSAGES
  #define TELEMETRY_CMD_MED       CMD_MED_FREQUENCY_MESSAGES
  #define TELEMETRY_CMD_HIGH      CMD_HIGH_FREQUENCY_MESSAGES
#else
  #define TELEMETRY_CLASS_LOW     MSG_SLAVE_LOW_FREQUENCY
  #define TELEMETRY_CLASS_MED     MSG_SLAVE_MED_FREQUENCY
  #define TELEMETRY_CLASS_HIGH    MSG_SLAVE_HIGH_FREQUENCY
  #define TELEMETRY_CMD_LOW       CMD_TELEMETRY_SLAVE_LOW_FREQUENCY
  #define TELEMETRY_CMD_MED       CMD_TELEMETRY_SLAVE_MED_FREQUENCY
  #define TELEMETRY_CMD_HIGH      CMD_TELEMETRY_SLAVE_HIGH_FREQUENCY
#endif

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern unsigned long arduinoLoopExecutionCount;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void handleCommonScheduledTasks();
