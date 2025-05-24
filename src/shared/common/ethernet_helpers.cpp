#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

#include "ethernet_helpers.h"
#include "general_helpers.h"
#include "variables.h"

/* ======================================================================
   VARIABLES: Ethernet related
   ====================================================================== */

// Define remote and local UDP ports
const uint16_t remoteArduinoListenPort = 8888;
const uint16_t localArduinoListenPort  = 8888;

// Define message sequence tracking and quality stats
unsigned long sendSequenceNumber           = 0;
unsigned long receiveSequenceNumber        = 0;
unsigned long sendMalformedMessageCount    = 0; // Detected but not sent
unsigned long receiveMalformedMessageCount = 0; // Detected and discarded

// Define ping tracking variables
unsigned long pingLastSequenceNumber;
unsigned long pingLastSentTime;

// Define the buffer for round trip time (RTT) measurements
unsigned long rttBuffer[RTT_BUFFER_SIZE];
uint8_t       rttIndex = 0;

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
  DEBUG_ETHERNET_GENERAL("INFO - Initialising ethernet shield ...");
  Ethernet.begin(config.mac, config.ip);

  int eth_status = Ethernet.hardwareStatus();

  if (eth_status == EthernetW5500) {
    DEBUG_ETHERNET_GENERAL("\tOK - W5500 Ethernet controller detected");
    ethUdpClient.begin(localArduinoListenPort); // Start listening on the local port
    DEBUG_ETHERNET_GENERAL("\tOK - Listening on local UDP port %u", localArduinoListenPort);
  } else if (eth_status != EthernetW5500) { // hardwareStatus() is a macro or enum constant defined as an int ... apparently
    DEBUG_ETHERNET_GENERAL("\tFATAL - Ethernet status is %s", eth_status);
  }
}

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
    DEBUG_ETHERNET_TRAFFIC("Malformed UDP packet received (checksum failed), discarding");
    return false;
  }

  // Find first and last comma
  char *firstComma = strchr(buffer, ',');
  char *lastComma  = strrchr(buffer, ',');

  if (!firstComma || !lastComma || firstComma == lastComma) {
    receiveMalformedMessageCount++;
    DEBUG_ETHERNET_TRAFFIC("Malformed UDP packet (missing required fields), discarding");
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
  DEBUG_ETHERNET_TRAFFIC("Valid UDP packet: seq=%lu, payload=%s", seq, buffer);
  return true;
}

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

// Validate the incoming UDP message checksum
bool validateIncomingUdpMessageChecksum(const char *message) {
  const char *lastComma = strrchr(message, ',');
  if (!lastComma || *(lastComma + 1) == '\0') {
    DEBUG_ETHERNET_TRAFFIC("Invalid message: no checksum found");
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
