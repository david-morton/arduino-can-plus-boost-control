#include "ethernet_ping.h"
#include "../helpers_logging.h"
#include "../variables.h"
#include "ethernet_send.h"


/* ======================================================================
   VARIABLES
   ====================================================================== */

// Define ping tracking variables
unsigned long pingLastSequenceNumber;
unsigned long pingLastSentTime;

// Define the buffer for round trip time (RTT) measurements
unsigned long rttBuffer[RTT_BUFFER_SIZE];
uint8_t       rttIndex = 0;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Send a ping request to the remote Arduino
bool sendArduinoPingRequest() {
  char pingRequestBuffer[32]; // Enough for "0,<seq>"
  snprintf(pingRequestBuffer, sizeof(pingRequestBuffer), "0,%lu", sendSequenceNumber);

  // Set state tracking variables
  pingLastSentTime       = micros();
  pingLastSequenceNumber = sendSequenceNumber;
  // Create message to send which is simply a command ID 0 with sequence number as payload
  if (sendUdpMessage(pingRequestBuffer)) {
    return true;
  }
}

// Send a ping response to the remote Arduino
bool sendPingResponseToRemoteArduino(int pingSeq) {
  char pingResponseBuffer[32]; // Enough for "1,<seq>"
  snprintf(pingResponseBuffer, sizeof(pingResponseBuffer), "1,%d", pingSeq);

  // Send the response
  if (sendUdpMessage(pingResponseBuffer)) {
    return true;
  } else {
    DEBUG_ETHERNET_PING("Failed to send ping response to remote Arduino %d", pingSeq);
    return false;
  }
}

// Handle a ping response from the remote Arduino
void handlePingResponse(unsigned long timestamp, unsigned long sequenceNumber) {
  int pingRountTripMs = timestamp - pingLastSentTime;
  logPingRtt(pingRountTripMs);
  DEBUG_ETHERNET_PING("Ping round trip time: %d ms, sequence number: %lu", pingRountTripMs, sequenceNumber);
}

// Log the ping round trip time in a circular buffer
void logPingRtt(unsigned long rtt) {
  rttBuffer[rttIndex++] = rtt;
  if (rttIndex >= RTT_BUFFER_SIZE) {
    rttIndex = 0; // Wrap around
  }
}

// Get the average round trip time (RTT) from the circular buffer
unsigned long getAveragePingRtt() {
  uint8_t count = rttIndex == 0 ? RTT_BUFFER_SIZE : rttIndex;

  unsigned long total = 0;
  for (uint8_t i = 0; i < count; i++) {
    total += rttBuffer[i];
  }
  return total / count;
}
