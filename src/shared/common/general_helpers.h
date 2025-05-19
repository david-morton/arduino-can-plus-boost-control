#pragma once

#include <Arduino.h>

/* ======================================================================
   HELPER: Global flags and functions for debugging
   ====================================================================== */
extern bool debugEthernetFunctionality;
extern bool debugEthernetTraffic;
extern bool debugGeneral;
extern bool debugGears;
extern bool debugLoopInfo;

/* ======================================================================
   HELPERS: Debug output definitions
   ====================================================================== */
// Define the DEBUG_ETHERNET_FUNCTIONALITY macro
#define DEBUG_ETHERNET_FUNCTIONALITY(message)          \
  do {                                                 \
    if (debugEthernetFunctionality) {                  \
      Serial.print("[DEBUG ETHERNET FUNCTIONALITY] "); \
      Serial.println(message);                         \
    }                                                  \
  } while (0)

// Define the DEBUG_ETHERNET_TRAFFIC macro
#define DEBUG_ETHERNET_TRAFFIC(message)          \
  do {                                           \
    if (debugEthernetTraffic) {                  \
      Serial.print("[DEBUG ETHERNET TRAFFIC] "); \
      Serial.println(message);                   \
    }                                            \
  } while (0)

// Define the DEBUG_GENERAL macro
#define DEBUG_GENERAL(message)          \
  do {                                  \
    if (debugGeneral) {                 \
      Serial.print("[DEBUG GENERAL] "); \
      Serial.println(message);          \
    }                                   \
  } while (0)

// Define the DEBUG_GEARS macro
#define DEBUG_GEARS(message)          \
  do {                                \
    if (debugGears) {                 \
      Serial.print("[DEBUG GEARS] "); \
      Serial.println(message);        \
    }                                 \
  } while (0)

// Define the DEBUG_LOOP_INFO macro
#define DEBUG_LOOP_INFO(message)          \
  do {                                    \
    if (debugLoopInfo) {                  \
      Serial.print("[DEBUG LOOP INFO] "); \
      Serial.print(message);              \
    }                                     \
  } while (0)

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */
int  getAveragedAnaloguePinReading(byte, int, int);
void reportArduinoLoopRate(unsigned long *);
