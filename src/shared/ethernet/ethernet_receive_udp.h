#pragma once

#include <Arduino.h>
#include <Ethernet.h>

// Define the packet buffer sizes in bytes (maximum size of UDP packets)
#define RECEIVE_PACKET_BUFFER_SIZE 256

extern char udpReceiveBuffer[RECEIVE_PACKET_BUFFER_SIZE];

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

bool parseIncomingUdpMessage(char *buffer, size_t bufferSize);
bool validateIncomingUdpMessageChecksum(const char *message);
void handleIncomingUdpMessage();
