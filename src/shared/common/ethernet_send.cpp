#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

#include "ethernet_send.h"
#include "helpers_logging.h"
#include "variables.h"

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
bool validateSendMessage(const char *message) {
  // Check if the message is too long
  if (strlen(message) > SEND_PACKET_BUFFER_SIZE - 1) {
    DEBUG_ETHERNET_TRAFFIC("ERROR - Message too long, not sending: %s", message);
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
  DEBUG_ETHERNET_TRAFFIC("Calculated checksum: %u", checksum);
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
  if (!validateSendMessage(udpMessage)) {
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
