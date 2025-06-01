#pragma once

#include <Arduino.h>

/* ======================================================================
   HELPER: Global flags and functions for debugging
   ====================================================================== */
extern bool debugError;
extern bool debugEthernetGeneral;
extern bool debugEthernetTraffic;
extern bool debugEthernetPing;
extern bool debugGeneral;
extern bool debugGears;
extern bool debugLightSensor;
extern bool debugPerformance;

/* ======================================================================
   HELPERS: Debug output macros using snprintf (safe, no String)
   ====================================================================== */
#define _DEBUG_PRINTF(enabled, tag, fmt, ...)           \
  do {                                                  \
    if (enabled) {                                      \
      char _buf[256];                                   \
      snprintf(_buf, sizeof(_buf), fmt, ##__VA_ARGS__); \
      Serial.print("[DEBUG ");                          \
      Serial.print(tag);                                \
      Serial.print("] ");                               \
      Serial.println(_buf);                             \
    }                                                   \
  } while (0)

#define DEBUG_ERROR(fmt, ...) \
  _DEBUG_PRINTF(debugError, "ERROR", fmt, ##__VA_ARGS__)

#define DEBUG_ETHERNET_GENERAL(fmt, ...) \
  _DEBUG_PRINTF(debugEthernetGeneral, "ETHERNET GENERAL", fmt, ##__VA_ARGS__)

#define DEBUG_ETHERNET_TRAFFIC(fmt, ...) \
  _DEBUG_PRINTF(debugEthernetTraffic, "ETHERNET TRAFFIC", fmt, ##__VA_ARGS__)

#define DEBUG_ETHERNET_PING(fmt, ...) \
  _DEBUG_PRINTF(debugEthernetPing, "ETHERNET PING", fmt, ##__VA_ARGS__)

#define DEBUG_GENERAL(fmt, ...) \
  _DEBUG_PRINTF(debugGeneral, "GENERAL", fmt, ##__VA_ARGS__)

#define DEBUG_GEARS(fmt, ...) \
  _DEBUG_PRINTF(debugGears, "GEARS", fmt, ##__VA_ARGS__)

#define DEBUG_LIGHT_SENSOR(fmt, ...) \
  _DEBUG_PRINTF(debugLightSensor, "LIGHT SENSOR", fmt, ##__VA_ARGS__)

#define DEBUG_PERFORMANCE(fmt, ...) \
  _DEBUG_PRINTF(debugPerformance, "PERFORMANCE", fmt, ##__VA_ARGS__)

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */
int  getAveragedAnaloguePinReading(byte, int, int);
void reportArduinoLoopRate(unsigned long *);
