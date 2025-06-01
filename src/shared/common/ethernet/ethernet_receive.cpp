#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

#include "../command_ids.h"
#include "../helpers_logging.h"
#include "../message_handlers_udp.h"
#include "../variables.h"
#include "ethernet_receive.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

// Define local UDP listen port
const uint16_t localArduinoListenPort = 8888;

// Define message sequence tracking and quality stats
unsigned long receiveSequenceNumber        = 0;
unsigned long receiveMalformedMessageCount = 0; // Detected and discarded

// Define UDP receive buffer
char udpReceiveBuffer[RECEIVE_PACKET_BUFFER_SIZE];

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Get incoming UDP message, validate it and save payload to buffer for processing
bool getIncomingUdpMessage(char *buffer, size_t bufferSize) {
  int packetSize = ethUdpClient.parsePacket();
  if (packetSize <= 0)
    return false;

  // Read raw UDP message
  int len = ethUdpClient.read(buffer, bufferSize - 1);
  if (len <= 0)
    return false;

  buffer[len] = '\0'; // Null-terminate the received string
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
  // Update sequence counter
  receiveSequenceNumber = seq;
  DEBUG_ETHERNET_TRAFFIC("Received valid UDP packet: seq=%lu, payload=%s", seq, buffer);
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
void processIncomingUdpMessages() {
  if (!getIncomingUdpMessage(udpReceiveBuffer, sizeof(udpReceiveBuffer))) {
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
      handleLowFrequencyMessages(payloadStr, strlen(payloadStr));
      break;

    default:
      DEBUG_ERROR("Unknown command ID: %d", commandId);
      break;
  }
}
