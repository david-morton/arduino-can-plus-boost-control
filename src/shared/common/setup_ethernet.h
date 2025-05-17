#pragma once

#include <Arduino.h>
#include <Ethernet.h>

/* ======================================================================
   GLOBALS: Used by other modules
   ====================================================================== */
extern EthernetClient ethClient;

/* ======================================================================
   CUSTOM STRUCTURES
   ====================================================================== */
struct EthernetConfig {
  byte mac[6];
  IPAddress ip;
};

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */
void initialiseEthernetShield(EthernetConfig& config);
