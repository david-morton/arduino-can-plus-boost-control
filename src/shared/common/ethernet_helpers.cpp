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

// Define the packet buffer size in bytes. This is the maximum size of the UDP packet that can be received
char packetBuffer[128];

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

  char eth_status = Ethernet.hardwareStatus();

  if (eth_status == EthernetW5500) {
    DEBUG_ETHERNET_FUNCTIONALITY("\tOK - W5500 Ethernet controller detected");
    ethUdpClient.begin(localArduinoListenPort); // Start listening on the local port
    DEBUG_ETHERNET_FUNCTIONALITY("\tOK - Listening on local UDP port " + String(localArduinoListenPort));
  } else if (eth_status != EthernetW5500) {
    DEBUG_ETHERNET_FUNCTIONALITY("\tFATAL - Ethernet status is " + String(eth_status));
  }
}

// Check for incoming UDP packets
bool checkForIncomingUdpMessage() {
  int packetSize = ethUdpClient.parsePacket();

  if (packetSize) {
    // Read the packet into the buffer
    int len = ethUdpClient.read(packetBuffer, sizeof(packetBuffer) - 1);
    if (len > 0) {
      packetBuffer[len] = '\0'; // Null-terminate the string
    }
    DEBUG_ETHERNET_TRAFFIC("INFO - Received UDP packet: " + String(packetBuffer));
    return true;
  }
  return false;
}

// Send a UDP packet to the remote Arduino
bool sendUdpMessage(const IPAddress &remoteArduinoIp, const char *message) {
  // Send the packet to the remote Arduino
  ethUdpClient.beginPacket(remoteArduinoIp, remoteArduinoListenPort);
  ethUdpClient.write(message);
  ethUdpClient.endPacket();
  DEBUG_ETHERNET_TRAFFIC("INFO - Sent UDP packet: " + String(message));
  return true;
}
