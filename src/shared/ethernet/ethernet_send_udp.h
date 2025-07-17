#pragma once

#include <Arduino.h>
#include <Ethernet.h>

/* ======================================================================
   CONSTANTS
   ====================================================================== */

// Define the packet buffer sizes in bytes (maximum size of UDP packets)
constexpr int SEND_PACKET_BUFFER_SIZE = 256;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

bool sendUdpMessage(const char *message);
void sendUdpMessageWithCommand(int commandId, const char *payload);
void reportUdpMessageSendStats();
