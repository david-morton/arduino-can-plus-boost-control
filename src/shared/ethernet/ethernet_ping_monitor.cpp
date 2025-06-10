#include "ethernet_ping_monitor.h"
#include "../debug_logging.h"
#include "../variables_programmatic.h"
#include "ethernet_send_udp.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

// Define ping tracking variables
unsigned long pingLastSequenceNumber;
unsigned long pingLastSentTimeMicros;

// Define the buffer and index for round trip time (RTT) measurements
unsigned long rttBuffer[RTT_BUFFER_SIZE];
uint8_t       rttIndex = 0;

// Define the sequence number and array for outstanding ping requests
PingResult pingHistory[MAX_OUTSTANDING_PINGS];
uint8_t    pingHistoryIndex = 0;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Send a ping response to the remote Arduino
bool sendPingResponseToRemoteArduino(int pingSeq) {
  char pingResponseBuffer[64]; // Enough for "1,<seq>"
  snprintf(pingResponseBuffer, sizeof(pingResponseBuffer), "1,%d", pingSeq);

  // Send the response
  if (sendUdpMessage(pingResponseBuffer)) {
    return true;
  } else {
    DEBUG_ERROR("Failed to send ping response to remote Arduino %d", pingSeq);
    return false;
  }
}

// Send a ping request to the remote Arduino
bool sendArduinoPingRequest() {
  unsigned long seq = sendSequenceNumber; // Already incremented inside sendUdpMessage()

  char pingRequestBuffer[32];
  snprintf(pingRequestBuffer, sizeof(pingRequestBuffer), "0,%lu", seq);

  if (!sendUdpMessage(pingRequestBuffer)) {
    return false;
  }

  // Always record this attempt in ring buffer
  pingHistory[pingHistoryIndex].sequence       = seq;
  pingHistory[pingHistoryIndex].sentTimeMicros = micros();
  pingHistory[pingHistoryIndex].responded      = false;

  DEBUG_ETHERNET_PING("Sent ping request with sequence number: %lu", seq);

  pingHistoryIndex = (pingHistoryIndex + 1) % MAX_OUTSTANDING_PINGS;
  return true;
}

// Process a ping response from the remote Arduino
void handlePingResponse(unsigned long timestampMicros, unsigned long sequenceNumber) {
  for (uint8_t i = 0; i < MAX_OUTSTANDING_PINGS; i++) {
    if (pingHistory[i].sequence == sequenceNumber && !pingHistory[i].responded) {
      unsigned long rtt              = timestampMicros - pingHistory[i].sentTimeMicros;
      pingHistory[i].responded       = true;
      pingHistory[i].roundTripMicros = rtt;

      if (rtt <= PING_TIMEOUT_MICROS) {
        handlePingRttBuffer(rtt);
        DEBUG_ETHERNET_PING("Ping RTT: %.2f ms, seq: %lu", rtt / 1000.0f, sequenceNumber);
      } else {
        DEBUG_ERROR("Ping response too late: %.2f ms, seq: %lu", rtt / 1000.0f, sequenceNumber);
      }
      return;
    }
  }

  DEBUG_ERROR("Unexpected or duplicate ping response, seq: %lu", sequenceNumber);
}

// Log the ping round trip time in a circular buffer
void handlePingRttBuffer(unsigned long rtt) {
  rttBuffer[rttIndex++] = rtt;
  if (rttIndex >= RTT_BUFFER_SIZE) {
    rttIndex = 0; // Wrap around
  }
}

// Get the average round trip time (RTT) from the circular buffer
unsigned long calculateAveragePingRttMicros() {
  uint8_t count = rttIndex == 0 ? RTT_BUFFER_SIZE : rttIndex;

  unsigned long total = 0;
  for (uint8_t i = 0; i < count; i++) {
    total += rttBuffer[i];
  }
  return total / count;
}

// Check for ping timeouts and calculate packet loss
void handlePingTimeoutsAndLoss() {
  unsigned long now   = micros();
  uint8_t       count = 0;
  uint8_t       lost  = 0;

  for (uint8_t i = 0; i < MAX_OUTSTANDING_PINGS; i++) {
    if (pingHistory[i].sequence == 0)
      continue; // skip unused entries

    count++;

    unsigned long age     = now - pingHistory[i].sentTimeMicros;
    bool          expired = !pingHistory[i].responded && age > PING_TIMEOUT_MICROS;

    if (expired)
      lost++;
  }

  if (count == 0)
    return;

  float avgRttMs    = calculateAveragePingRttMicros() / 1000.0f;
  float lossPercent = (lost * 100.0f) / count;

  if (lossPercent >= 25.0f) {
    DEBUG_ERROR("ERROR: High ping loss detected: %.1f%%", lossPercent);
  } else
    DEBUG_ETHERNET_GENERAL("Ping RTT: %.2f ms | Loss: %.1f%%", avgRttMs, lossPercent);
}
