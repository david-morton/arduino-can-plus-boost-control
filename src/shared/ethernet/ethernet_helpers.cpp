#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

#include "../debug_logging.h"
#include "../variables_programmatic.h"
#include "ethernet_helpers.h"

/* ======================================================================
   VARIABLES
   ====================================================================== */

bool globalHealthEthernetPeerOnline = true;
bool globalHealthEthernetConnected  = true;

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
  pinMode(config.csPin, OUTPUT);
  Ethernet.begin(config.mac, config.ip);

  int eth_status = Ethernet.hardwareStatus();

  if (eth_status == EthernetW5500) {
    DEBUG_GENERAL("\t\tW5500 Ethernet controller detected");
    Ethernet.init(config.csPin);
    ethUdpClient.begin(localArduinoListenPort); // Start listening on the local port
    DEBUG_GENERAL("\t\tListening on local UDP port %u", localArduinoListenPort);
  } else if (eth_status != EthernetW5500) {
    DEBUG_ERROR("\tEthernet setup status is %s", eth_status);
  }
}

// Check if the Ethernet shield is connected and ready
void updateEthernetLinkStatus() {
  static EthernetLinkStatus lastLinkStatus = LinkON;
  EthernetLinkStatus        link_status    = Ethernet.linkStatus();

  if (link_status == LinkON && lastLinkStatus != LinkON) {
    globalHealthEthernetConnected = true;
    DEBUG_ERROR("Ethernet shield is now connected and link is ON.");
  } else if (link_status == LinkOFF && lastLinkStatus != LinkOFF) {
    globalHealthEthernetConnected = false;
    DEBUG_ERROR("Ethernet shield does not have a link or is not connected.");
  }
  lastLinkStatus = link_status;
}
