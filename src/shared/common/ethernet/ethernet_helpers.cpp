#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

#include "ethernet_helpers.h"
#include "../helpers_logging.h"
#include "../variables.h"

/* ======================================================================
   CONFIGURE CLIENT
   ====================================================================== */

EthernetClient ethClient;
EthernetUDP    ethUdpClient;

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Initialise the ethernet shield ready for use
void initialiseEthernetShield(EthernetConfig &config) {
  DEBUG_ETHERNET_GENERAL("INFO - Initialising ethernet shield ...");
  Ethernet.begin(config.mac, config.ip);

  int eth_status = Ethernet.hardwareStatus();

  if (eth_status == EthernetW5500) {
    DEBUG_ETHERNET_GENERAL("\tOK - W5500 Ethernet controller detected");
    ethUdpClient.begin(localArduinoListenPort); // Start listening on the local port
    DEBUG_ETHERNET_GENERAL("\tOK - Listening on local UDP port %u", localArduinoListenPort);
  } else if (eth_status != EthernetW5500) { // hardwareStatus() is a macro or enum constant defined as an int ... apparently
    DEBUG_ETHERNET_GENERAL("\tFATAL - Ethernet status is %s", eth_status);
  }
}
