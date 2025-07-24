#pragma once

#include <Arduino.h>
#include <Ethernet.h>

/* ======================================================================
   STRUCTURES
   ====================================================================== */

struct EthernetConfig {
  byte      mac[6];
  IPAddress ip;
  int       csPin;
};

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern bool           globalHealthEthernetConnected;
extern bool           globalHealthEthernetPeerOnline;
extern EthernetClient ethClient;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void initialiseEthernetShield(EthernetConfig &config);
void updateEthernetLinkStatus();
