#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

#include "ethernet_helpers.h"
#include "general_helpers.h"

/* ======================================================================
   VARIABLES: Ethernet related
   ====================================================================== */

// Define remote listening port for outbound Arduino messages
const uint16_t remoteArduinoListenPort = 8888;

// Define local listening port for inbound Arduino messages
const uint16_t localArduinoListenPort = 8888;

// Define the packet buffer size in bytes. This is the maximum size of the UDP packet that can be stored
const size_t sendPacketBufferSize    = 256;
const size_t receivePacketBufferSize = 256;

// Define variables for tracking message quality stats
uint32_t sendSequenceNumber           = 0;
uint32_t receiveSequenceNumber        = 0;
uint32_t sendMalformedMessageCount    = 0; // These are detected, but not sent
uint32_t receiveMalformedMessageCount = 0; // These are detected, and discarded

/* ======================================================================
   CONFIGURE CLIENT
   ====================================================================== */

EthernetClient ethClient;
EthernetUDP    ethUdpClient;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Initialise the ethernet shield ready for use
void initialiseEthernetShield(EthernetConfig &config) {
  DEBUG_ETHERNET_FUNCTIONALITY("INFO - Initialising ethernet shield ...");
  Ethernet.begin(config.mac, config.ip);

  int eth_status = Ethernet.hardwareStatus();

  if (eth_status == EthernetW5500) {
    DEBUG_ETHERNET_FUNCTIONALITY("\tOK - W5500 Ethernet controller detected");
    ethUdpClient.begin(localArduinoListenPort); // Start listening on the local port
    DEBUG_ETHERNET_FUNCTIONALITY("\tOK - Listening on local UDP port %u", localArduinoListenPort);
  } else if (eth_status != EthernetW5500) { // hardwareStatus() is a macro or enum constant defined as an int ... apparently
    DEBUG_ETHERNET_FUNCTIONALITY("\tFATAL - Ethernet status is %s", eth_status);
  }
}

// Check for incoming UDP packets
bool checkForIncomingUdpMessage() {
  int packetSize = ethUdpClient.parsePacket();

  if (packetSize) {
    // Declare the packet buffer
    char receivePacketBuffer[receivePacketBufferSize];
    // Read the packet into the buffer
    int len = ethUdpClient.read(receivePacketBuffer, sizeof(receivePacketBuffer) - 1);
    if (len > 0) {
      receivePacketBuffer[len] = '\0'; // Null-terminate the string
    }
    DEBUG_ETHERNET_TRAFFIC("Received UDP packet: %s", receivePacketBuffer);
    return true;
  }
  return false;
}

// Validate the message to be sent will fit within the buffer
bool validateSendMessage(const char *message) {
  // Check if the message is too long
  if (strlen(message) > sendPacketBufferSize - 1) {
    DEBUG_ETHERNET_TRAFFIC("ERROR: Message too long, not sending: %s", message);
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
  char messageAndSequence[sendPacketBufferSize - 32];
  snprintf(messageAndSequence, sizeof(messageAndSequence), "%lu,%s", sequenceNumber, payload);
  uint16_t calculatedChecksum = calculateSendChecksum(messageAndSequence);
  snprintf(out, outSize, "%s,%u", messageAndSequence, calculatedChecksum);
  DEBUG_ETHERNET_TRAFFIC("Built UDP message: %s", out);
}

// Send a UDP packet to the remote Arduino
bool sendUdpMessage(const IPAddress &remoteArduinoIp, const char *message) {
  // Build up the message to be sent
  char udpMessage[sendPacketBufferSize];
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
