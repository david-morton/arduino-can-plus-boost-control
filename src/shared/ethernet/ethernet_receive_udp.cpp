#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

#include "../command_ids.h"
#include "../debug_logging.h"
#include "../telemetry_receive_parser.h"
#include "../udp_command_dispatcher.h"
#include "../variables_programmatic.h"
#include "ethernet_receive_udp.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

// Define local UDP listen port
const uint16_t localArduinoListenPort = 8888;

// Define message sequence tracking and quality stats
unsigned long receiveOutOfSequenceCount    = 0;
unsigned long receiveMalformedMessageCount = 0;

unsigned long receiveLastSequenceNumber   = 0;
unsigned long receiveUdpTotalMessageCount = 0;

// Define UDP receive buffer
char udpReceiveBuffer[RECEIVE_PACKET_BUFFER_SIZE];

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Get incoming UDP message, validate it and save payload to buffer for processing
bool parseIncomingUdpMessage(char *buffer, size_t bufferSize) {
  int packetSize = ethUdpClient.parsePacket();
  if (packetSize <= 0)
    return false;

  // Read raw UDP message
  int len = ethUdpClient.read(buffer, bufferSize - 1);
  if (len <= 0)
    return false;

  buffer[len] = '\0'; // Null-terminate the received string
  receiveUdpTotalMessageCount++;
  DEBUG_ETHERNET_TRAFFIC("Received UDP packet: %s", buffer);

  // Validate checksum using the full message string
  if (!validateIncomingUdpMessageChecksum(buffer)) {
    receiveMalformedMessageCount++;
    DEBUG_ERROR("Malformed UDP packet received (checksum failed), discarding");
    return false;
  }

  // Find first and last comma
  char *firstComma = strchr(buffer, ',');
  char *lastComma  = strrchr(buffer, ',');

  if (!firstComma || !lastComma || firstComma == lastComma) {
    receiveMalformedMessageCount++;
    DEBUG_ERROR("Malformed UDP packet (missing required fields), discarding");
    return false;
  }

  // Extract sequence number
  *firstComma       = '\0'; // Terminate seq token
  unsigned long seq = strtoul(buffer, nullptr, 10);
  // Extract checksum
  const char *checksumStr = lastComma + 1;
  // Trim checksum from original buffer
  *lastComma = '\0'; // Now buffer = "<seq>,<payload>"
  // Extract payload (everything between first and last comma)
  const char *payload = firstComma + 1;
  // Copy payload back into caller's buffer
  strncpy(buffer, payload, bufferSize - 1);
  buffer[bufferSize - 1] = '\0'; // Safety null-termination
  DEBUG_ETHERNET_TRAFFIC("Received valid UDP packet: seq=%lu, payload=%s", seq, buffer);

  // Check if message was out of sequence
  if (receiveLastSequenceNumber + 1 != seq && receiveLastSequenceNumber != 0) {
    receiveOutOfSequenceCount++;
    DEBUG_ERROR("Received out-of-sequence UDP packet: expected %lu, got %lu", receiveLastSequenceNumber + 1, seq);
  }

  // Update last sequence number for next comparison
  receiveLastSequenceNumber = seq;

  return true;
}

// Validate the incoming UDP message checksum
bool validateIncomingUdpMessageChecksum(const char *message) {
  const char *lastComma = strrchr(message, ',');
  if (!lastComma || *(lastComma + 1) == '\0') {
    DEBUG_ERROR("Invalid message: no checksum found");
    return false;
  }

  // Extract numeric checksum
  uint16_t receivedChecksum = strtoul(lastComma + 1, nullptr, 10);

  // Calculate checksum over the part *before* the final comma
  size_t   lengthToCheck      = lastComma - message;
  uint16_t calculatedChecksum = 0;
  for (size_t i = 0; i < lengthToCheck; i++) {
    calculatedChecksum += message[i];
  }

  return calculatedChecksum == receivedChecksum;
}

// Process incoming UDP messages
void handleIncomingUdpMessage() {
  if (!parseIncomingUdpMessage(udpReceiveBuffer, sizeof(udpReceiveBuffer))) {
    return; // No packet received
  }

  // Extract command ID
  char *commandIdStr = strtok(udpReceiveBuffer, ",");
  if (!commandIdStr) {
    DEBUG_ERROR("Malformed UDP payload: missing command ID");
    return;
  }
  int commandId = atoi(commandIdStr);

  // Extract payload (rest of string)
  char *payloadStr = strtok(nullptr, "");

  switch (commandId) {
    case CMD_RECEIVE_PING_REQUEST:
      handlePingRequestOrResponse(0, payloadStr, strlen(payloadStr));
      break;

    case CMD_RECEIVE_PING_RESPONSE:
      handlePingRequestOrResponse(1, payloadStr, strlen(payloadStr));
      break;

    case CMD_LOW_FREQUENCY_MESSAGES:
      parseTelemetryPayload(payloadStr);
      break;

    case CMD_MED_FREQUENCY_MESSAGES:
      parseTelemetryPayload(payloadStr);
      break;

    case CMD_HIGH_FREQUENCY_MESSAGES:
      parseTelemetryPayload(payloadStr);
      break;

    default:
      DEBUG_ERROR("Unknown command ID in UDP receive: %d", commandId);
      break;
  }
}

// Report on UDP received message statistics
void reportUdpMessageReceiveStats() {
  float malformedPercentage     = (receiveUdpTotalMessageCount > 0) ? (receiveMalformedMessageCount * 100.0f / receiveUdpTotalMessageCount) : 0.0f;
  float outOfSequencePercentage = (receiveUdpTotalMessageCount > 0) ? (receiveOutOfSequenceCount * 100.0f / receiveUdpTotalMessageCount) : 0.0f;

  DEBUG_PERFORMANCE("UDP messages received: %lu, Malformed: %.2f%% (%lu), Out of sequence: %.2f%% (%lu)",
                    receiveUdpTotalMessageCount, malformedPercentage, receiveMalformedMessageCount,
                    outOfSequencePercentage, receiveOutOfSequenceCount);
}
