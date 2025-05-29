#include "message_handlers_udp.h"
#include "../common/ethernet/ethernet_ping.h"
#include "../common/ethernet/ethernet_send.h"
#include "helpers_logging.h"
#include "variables.h"

// Handle ping request (command ID 0) from remote Arduino (ie: we need to send a response)
// Handle ping response (command ID 1) from remote Arduino (ie: we need to process the response, working out the RTT)
bool handlePingRequestOrResponse(int cmdId, const char *payload, size_t payloadLength) {
  char temp[payloadLength + 1];
  memcpy(temp, payload, payloadLength);
  temp[payloadLength] = '\0';

  char *seqStr = strtok(temp, ",");
  if (!seqStr) {
    DEBUG_ETHERNET_PING("Malformed ping payload");
    return false;
  }

  int pingSeq = atoi(seqStr);

  if (cmdId == 0) { // Ping request
    DEBUG_ETHERNET_PING("Received ping request with sequence number: %d", pingSeq);
    return sendPingResponseToRemoteArduino(pingSeq);
  } else if (cmdId == 1) { // Ping response
    DEBUG_ETHERNET_PING("Received ping response with sequence number: %d", pingSeq);
    processPingResponse(micros(), pingSeq);
  }
}
