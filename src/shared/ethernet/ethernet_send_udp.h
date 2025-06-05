#pragma once

#include <Arduino.h>
#include <Ethernet.h>

// Define the packet buffer sizes in bytes (maximum size of UDP packets)
#define SEND_PACKET_BUFFER_SIZE 256

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */
bool sendUdpMessage(const char *message);
void sendUdpMessageWithCommand(int commandId, const char *payload);
