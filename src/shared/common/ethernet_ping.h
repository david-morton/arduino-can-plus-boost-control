#pragma once

#include <Arduino.h>
#include <Ethernet.h>

// Define the moving window array size for ping response times
#define RTT_BUFFER_SIZE 50

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */
bool          sendArduinoPingRequest();
bool          sendPingResponseToRemoteArduino(int pingSeq);
unsigned long getAveragePingRtt();
void          handlePingResponse(unsigned long timestamp, unsigned long sequenceNumber);
void          logPingRtt(unsigned long rtt);
