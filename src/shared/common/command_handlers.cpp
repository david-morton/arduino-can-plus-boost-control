#include <Arduino.h>
#include "command_handlers.h"
#include "helpers_logging.h"

void handlePingRequest(const char* payload) {
  int pingSeq = atoi(payload);
  DEBUG_ETHERNET_PING("Ping request received with seq: %d", pingSeq);
}

void handlePingResponse(const char* payload) {
  int pingSeq = atoi(payload);
  DEBUG_ETHERNET_PING("Ping response received with seq: %d", pingSeq);
}
