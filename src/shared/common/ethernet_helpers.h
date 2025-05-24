#pragma once

#include <Arduino.h>
#include <Ethernet.h>

// Define the packet buffer sizes in bytes (maximum size of UDP packets)
#define RECEIVE_PACKET_BUFFER_SIZE 256
#define SEND_PACKET_BUFFER_SIZE 256

// Define the moving window array size for ping response times
#define RTT_BUFFER_SIZE 50

/* ======================================================================
   CONFIGURE THINGS
   ====================================================================== */
extern EthernetClient ethClient;

struct EthernetConfig {
  byte      mac[6];
  IPAddress ip;
};

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */
bool          getIncomingUdpMessage(char *buffer, size_t bufferSize);
bool          sendArduinoPingRequest();
bool          sendPingResponseToRemoteArduino(int pingSeq);
bool          sendUdpMessage(const IPAddress &remoteIp, const char *message);
bool          validateIncomingUdpMessageChecksum(const char *message);
unsigned long getAveragePingRtt();
void          handlePingResponse(unsigned long timestamp, unsigned long sequenceNumber);
void          initialiseEthernetShield(EthernetConfig &config);
void          logPingRtt(unsigned long rtt);
