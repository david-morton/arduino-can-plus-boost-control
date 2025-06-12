#pragma once

#include <Arduino.h>
#include <Ethernet.h>

/* ======================================================================
   CONFIGURE THINGS
   ====================================================================== */

extern EthernetClient ethClient;

struct EthernetConfig {
  byte      mac[6];
  IPAddress ip;
};

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void initialiseEthernetShield(EthernetConfig &config);
