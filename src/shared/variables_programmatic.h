#include <Ethernet.h>
#include <EthernetUdp.h>

#include <Arduino.h>

/* ======================================================================
   VARIABLES: These are used across both master and slave Arduinos
   ====================================================================== */

// Defined or derived variables
extern bool            remoteArduinoResponding;
extern const IPAddress remoteArduinoIp;
extern const uint16_t  localArduinoListenPort;

// General use variables
extern unsigned long sendSequenceNumber;

// Clients
extern EthernetClient ethClient;
extern EthernetUDP    ethUdpClient;
