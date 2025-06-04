#pragma once

#include <Arduino.h>
#include <Ethernet.h>

// Define the moving window array size for ping response times
#define RTT_BUFFER_SIZE 50

#define MAX_OUTSTANDING_PINGS 20
#define PING_TIMEOUT_MICROS 1000000UL // 5 seconds

/* ======================================================================
   STRUCTURES
   ====================================================================== */
typedef struct {
  unsigned long sequence;
  unsigned long sentTimeMicros;
  unsigned long roundTripMicros;
  bool          responded;
} PingResult;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */
bool          sendArduinoPingRequest();
bool          sendPingResponseToRemoteArduino(int pingSeq);
unsigned long getAveragePingRttMicros();
void          processPingResponse(unsigned long timestamp, unsigned long sequenceNumber);
void          updatePingRttBuffer(unsigned long rtt);
void          checkPingTimeoutsAndLoss();
