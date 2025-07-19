#pragma once

/* ======================================================================
   ENUM DEFINITIONS
   ====================================================================== */

// Payload definitions / IDs used for both sending and receiving messages.
typedef enum {
  PAYLOAD_ID_RECEIVE_PING_REQUEST = 0,
  PAYLOAD_ID_RECEIVE_PING_RESPONSE,
  PAYLOAD_ID_LOW_FREQUENCY_MESSAGES,
  PAYLOAD_ID_MED_FREQUENCY_MESSAGES,
  PAYLOAD_ID_HIGH_FREQUENCY_MESSAGES,
  PAYLOAD_ID_SYSTEM_DATA,
  PAYLOAD_ID_COUNT // Always last — indicates total number of commands
} CommandID;
