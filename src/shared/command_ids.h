#pragma once

/* ======================================================================
   ENUMS
   ====================================================================== */

// Mapping of command IDs to their respective commands
typedef enum {
  CMD_RECEIVE_PING_REQUEST   = 0,
  CMD_RECEIVE_PING_RESPONSE  = 1,
  CMD_LOW_FREQUENCY_MESSAGES = 2
} CommandID;
