#pragma once

#include <Arduino.h>
#include <Ethernet.h>

/* ======================================================================
   CONSTANTS
   ====================================================================== */

// Define the moving window array size for ping response times.
// This should be set relative to the ping frequency and sensitivity of setting an alarm condition.
constexpr int RTT_BUFFER_SIZE = 20;

// Define the maximum number of outstanding pings that can be tracked and the timeout period for a ping response.
constexpr int           MAX_OUTSTANDING_PINGS = 20;
constexpr unsigned long PING_TIMEOUT_MICROS   = 2000000UL; // 2 seconds

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
unsigned long calculateAveragePingRttMicros();
void          handlePingResponse(unsigned long timestamp, unsigned long sequenceNumber);
void          handlePingRttBuffer(unsigned long rtt);
void          handlePingTimeoutsAndLoss();
