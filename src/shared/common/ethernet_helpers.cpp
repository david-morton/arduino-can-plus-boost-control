#include <Ethernet.h>
#include <SPI.h>

#include "ethernet_helpers.h"

/* ======================================================================
   CONFIGURE THINGS
   ====================================================================== */
EthernetClient ethClient;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Initialise the ethernet shield ready for use
void initialiseEthernetShield(EthernetConfig &config) {
  Serial.println("INFO - Initialising ethernet shield");
  Ethernet.begin(config.mac, config.ip);

  char eth_status = Ethernet.hardwareStatus();

  if (eth_status == EthernetW5500) {
    Serial.println("\tOK - W5500 Ethernet controller detected");
  } else if (eth_status != EthernetW5500) {
    Serial.print("\tFATAL - Ethernet status is ");
    Serial.println(eth_status);
  }
}
