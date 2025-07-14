#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

#include "../debug_logging.h"
#include "../variables_programmatic.h"
#include "ethernet_helpers.h"

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
  DEBUG_GENERAL("Initialising ethernet shield ...");
  Ethernet.begin(config.mac, config.ip);

  int eth_status = Ethernet.hardwareStatus();

  if (eth_status == EthernetW5500) {
    DEBUG_GENERAL("\t\tW5500 Ethernet controller detected");
    ethUdpClient.begin(localArduinoListenPort); // Start listening on the local port
    DEBUG_GENERAL("\t\tListening on local UDP port %u", localArduinoListenPort);
  } else if (eth_status != EthernetW5500) { // hardwareStatus() is a macro or enum constant defined as an int ... apparently
    DEBUG_ERROR("\tEthernet setup status is %s", eth_status);
  }
}
