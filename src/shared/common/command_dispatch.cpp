#include <Arduino.h>
#include "command_ids.h"
#include "command_handlers.h"
#include "command_dispatch.h"

void dispatchCommand(int commandId, const char* payload) {
  switch (commandId) {
    case CMD_PING_REQUEST:
      handlePingRequest(payload);
      break;
    case CMD_PING_RESPONSE:
      handlePingResponse(payload);
      break;
    default:
      Serial.print("Unknown command ID: ");
      Serial.println(commandId);
      break;
  }
}
