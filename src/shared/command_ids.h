#pragma once

/* ======================================================================
   ENUMS
   ====================================================================== */

// Mapping of command IDs to their respective commands
typedef enum {
  CMD_RECEIVE_PING_REQUEST = 0,
  CMD_RECEIVE_PING_RESPONSE,
  CMD_LOW_FREQUENCY_MESSAGES,
  CMD_MED_FREQUENCY_MESSAGES,
  CMD_HIGH_FREQUENCY_MESSAGES,
  CMD_SYSTEM_DATA,
  CMD_COUNT // Always last — indicates total number of commands
} CommandID;
