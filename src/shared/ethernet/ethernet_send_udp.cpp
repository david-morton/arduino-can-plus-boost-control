#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

#include "../debug_logging.h"
#include "../variables_programmatic.h"
#include "ethernet_send_udp.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

// Define remote UDP listen port
const uint16_t remoteArduinoListenPort = 8888;

// Define message sequence tracking and quality stats
unsigned long sendSequenceNumber        = 0;
unsigned long sendMalformedMessageCount = 0; // Detected but not sent

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Validate the message to be sent will fit within the buffer
bool validateMessageLength(const char *message) {
  // Check if the message is too long
  if (strlen(message) > SEND_PACKET_BUFFER_SIZE - 1) {
    DEBUG_ERROR("UDP Message too long, not sending: %s", message);
    return false;
  }
  return true;
}

// Calculate the message sending checksum, including the sequence number
uint16_t calculateSendChecksum(const char *message) {
  uint16_t checksum = 0;
  for (size_t i = 0; i < strlen(message); i++) {
    checksum += message[i];
  }
  DEBUG_ETHERNET_TRAFFIC("Calculated UDP message checksum: %u", checksum);
  return checksum;
}

// Take a payload and sequence number and build a UDP message, including the checksum
// The message is formatted as: <sequenceNumber>,<payload>,<checksum>
void buildUdpMessage(char *out, size_t outSize, const char *payload, uint32_t sequenceNumber) {
  char messageAndSequence[SEND_PACKET_BUFFER_SIZE - 32];
  snprintf(messageAndSequence, sizeof(messageAndSequence), "%lu,%s", sequenceNumber, payload);
  uint16_t calculatedChecksum = calculateSendChecksum(messageAndSequence);
  snprintf(out, outSize, "%s,%u", messageAndSequence, calculatedChecksum);
  DEBUG_ETHERNET_TRAFFIC("Built UDP message: %s", out);
}

// Send a UDP packet to the remote Arduino
bool sendUdpMessage(const char *message) {
  // Build up the message to be sent
  char udpMessage[SEND_PACKET_BUFFER_SIZE];
  buildUdpMessage(udpMessage, sizeof(udpMessage), message, sendSequenceNumber);

  // Validate the message length before sending
  if (!validateMessageLength(udpMessage)) {
    sendMalformedMessageCount++;
    return false;
  }

  // Send the packet to the remote Arduino
  ethUdpClient.beginPacket(remoteArduinoIp, remoteArduinoListenPort);
  ethUdpClient.write(udpMessage);
  ethUdpClient.endPacket();
  DEBUG_ETHERNET_TRAFFIC("Sent UDP packet: %s", udpMessage);

  // Increment the sequence number for the next message
  sendSequenceNumber++;
  return true;
}

// Send a UDP message with a specific command ID
void sendUdpMessageWithCommand(int commandId, const char *payload) {
  char udpMessage[SEND_PACKET_BUFFER_SIZE];
  snprintf(udpMessage, sizeof(udpMessage), "%d,%s", commandId, payload);
  sendUdpMessage(udpMessage);
  DEBUG_ETHERNET_MESSAGES("Sent UDP message with command ID %d: %s", commandId, udpMessage);
}

// Report the statistics of sent UDP messages
void reportUdpMessageSendStats() {
  DEBUG_PERFORMANCE("UDP messages sent: %lu, Malformed: %lu", sendSequenceNumber, sendMalformedMessageCount);
}
