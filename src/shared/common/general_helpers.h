#pragma once

#include <Arduino.h>

/* ======================================================================
   HELPER: Global flags and functions for debugging
   ====================================================================== */
extern bool debugEthernetReceive;
extern bool debugEthernetSend;
extern bool debugGeneral;
extern bool debugGears;
extern bool debugLoopInfo;

/* ======================================================================
   HELPERS: Debug output definitions
   ====================================================================== */
// Define the DEBUG_ETHERNET_RECEIVE macro
#define DEBUG_ETHERNET_RECEIVE(message)        \
  do {                                         \
    if (debugEthernetReceive) {                \
      Serial.print("[DEBUG SERIAL RECEIVE] "); \
      Serial.println(message);                 \
    }                                          \
  } while (0)

// Define the DEBUG_ETHERNET_SEND macro
#define DEBUG_ETHERNET_SEND(message)        \
  do {                                      \
    if (debugEthernetSend) {                \
      Serial.print("[DEBUG SERIAL SEND] "); \
      Serial.println(message);              \
    }                                       \
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
