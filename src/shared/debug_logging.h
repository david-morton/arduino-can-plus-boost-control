#pragma once

#include <Arduino.h>

/* ======================================================================
   HELPER: Global flags and functions for debugging
   ====================================================================== */

extern bool debugCanBmw;
extern bool debugCanNissan;
extern bool debugError;
extern bool debugEthernetGeneral;
extern bool debugEthernetMessages;
extern bool debugEthernetPing;
extern bool debugEthernetTraffic;
extern bool debugGears;
extern bool debugGeneral;
extern bool debugMux;
extern bool debugPerformance;
extern bool debugSensorReadings;
extern bool debugTelemetry;

/* ======================================================================
   HELPER FUNCTION: Print timestamps
   ====================================================================== */

inline void printDebugTimestamp() {
  unsigned long ms    = millis();
  unsigned long sec   = ms / 1000;
  unsigned long milli = ms % 1000;
  unsigned long mins  = sec / 60;
  unsigned long hours = mins / 60;

  sec  = sec % 60;
  mins = mins % 60;

  char buf[20];
  snprintf(buf, sizeof(buf), "[%02lu:%02lu:%02lu.%03lu] ", hours, mins, sec, milli);
  Serial.print(buf);
}

/* ======================================================================
   HELPERS: Debug output macros using snprintf (safe, no String)
   ====================================================================== */

#define _DEBUG_PRINTF(enabled, tag, fmt, ...)           \
  do {                                                  \
    if (enabled) {                                      \
      char _buf[256];                                   \
      snprintf(_buf, sizeof(_buf), fmt, ##__VA_ARGS__); \
      printDebugTimestamp();                            \
      Serial.print("[DEBUG ");                          \
      Serial.print(tag);                                \
      Serial.print("] ");                               \
      Serial.println(_buf);                             \
    }                                                   \
  } while (0)

#define DEBUG_CAN_BMW(fmt, ...) \
  _DEBUG_PRINTF(debugCanBmw, "CAN BMW", fmt, ##__VA_ARGS__)

#define DEBUG_CAN_NISSAN(fmt, ...) \
  _DEBUG_PRINTF(debugCanNissan, "CAN NISSAN", fmt, ##__VA_ARGS__)

#define DEBUG_ERROR(fmt, ...) \
  _DEBUG_PRINTF(debugError, "ERROR", fmt, ##__VA_ARGS__)

#define DEBUG_ETHERNET_GENERAL(fmt, ...) \
  _DEBUG_PRINTF(debugEthernetGeneral, "ETHERNET GENERAL", fmt, ##__VA_ARGS__)

#define DEBUG_ETHERNET_MESSAGES(fmt, ...) \
  _DEBUG_PRINTF(debugEthernetMessages, "ETHERNET MESSAGES", fmt, ##__VA_ARGS__)

#define DEBUG_ETHERNET_TRAFFIC(fmt, ...) \
  _DEBUG_PRINTF(debugEthernetTraffic, "ETHERNET TRAFFIC", fmt, ##__VA_ARGS__)

#define DEBUG_ETHERNET_PING(fmt, ...) \
  _DEBUG_PRINTF(debugEthernetPing, "ETHERNET PING", fmt, ##__VA_ARGS__)

#define DEBUG_GENERAL(fmt, ...) \
  _DEBUG_PRINTF(debugGeneral, "GENERAL", fmt, ##__VA_ARGS__)

#define DEBUG_GEARS(fmt, ...) \
  _DEBUG_PRINTF(debugGears, "GEARS", fmt, ##__VA_ARGS__)

#define DEBUG_MUX(fmt, ...) \
  _DEBUG_PRINTF(debugMux, "MUX", fmt, ##__VA_ARGS__)

#define DEBUG_SENSOR_READINGS(fmt, ...) \
  _DEBUG_PRINTF(debugSensorReadings, "SENSOR READINGS", fmt, ##__VA_ARGS__)

#define DEBUG_PERFORMANCE(fmt, ...) \
  _DEBUG_PRINTF(debugPerformance, "PERFORMANCE", fmt, ##__VA_ARGS__)

#define DEBUG_TELEMETRY(fmt, ...) \
  _DEBUG_PRINTF(debugTelemetry, "TELEMETRY", fmt, ##__VA_ARGS__)

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void reportArduinoLoopRate(unsigned long *loopCounter);
