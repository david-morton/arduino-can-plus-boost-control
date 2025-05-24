#include <Arduino.h>
#include <ptScheduler.h> // The task scheduling library of choice

#include "shared/common/ethernet_helpers.h"
#include "shared/common/general_helpers.h"
#include "shared/common/variables.h"

#define CAN_2515

/* ======================================================================
   VARIABLES: Major functional area toggles
   ====================================================================== */

/* ======================================================================
   VARIABLES: Debug and stat output
   ====================================================================== */
bool debugEthernetGeneral = false;
bool debugEthernetTraffic = false;
bool debugEthernetPing    = false;
bool debugGears           = false;
bool debugGeneral         = false;
bool debugLoopInfo        = false;

/* ======================================================================
   VARIABLES: Ethernet and communication related
   ====================================================================== */

// Define MAC address and IP address for local Arduino
EthernetConfig ethConfigLocal = {
    .mac = {0xA8, 0x61, 0x0A, 0xAE, 0x1F, 0xF4},
    .ip  = IPAddress(192, 168, 10, 101)};

// Define remote IP address for peer Arduino native messeging
const IPAddress remoteArduinoIp(192, 168, 10, 100);

// Define UDP receive buffer
char udpReceiveBuffer[SEND_PACKET_BUFFER_SIZE];

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */

unsigned long arduinoLoopExecutionCount = 0;

/* ======================================================================
   OBJECTS: Pretty tiny scheduler objects / tasks
   ====================================================================== */
// High frequency tasks

// Medium frequency tasks

// Low frequency tasks
ptScheduler ptReportArduinoLoopStats = ptScheduler(PT_TIME_5S);

/* ======================================================================
   SETUP
   ====================================================================== */
void setup() {
  Serial.begin(500000); // Use a high baud rate for faster serial communication

  DEBUG_GENERAL("INFO: Entering main setup phase ...");

  initialiseEthernetShield(ethConfigLocal);
}

/* ======================================================================
   MAIN LOOP
   ====================================================================== */
void loop() {

  // Check for incoming UDP packets
  if (getIncomingUdpMessage(udpReceiveBuffer, sizeof(udpReceiveBuffer))) {
    // Extract command ID
    char *commandIdStr = strtok(udpReceiveBuffer, ",");
    if (!commandIdStr) {
      DEBUG_GENERAL("Malformed UDP payload: missing command ID");
      return;
    }

    int commandId = atoi(commandIdStr);

    // Dispatch by command ID
    switch (commandId) {
      case 0: { // Ping request
        // Extract sender's sendSequenceNumber used as ping identifier
        char *pingSeqStr = strtok(nullptr, ",");
        sendPingResponseToRemoteArduino(atoi(pingSeqStr));
      }

      default:
        DEBUG_GENERAL("Unknown command ID: %d", commandId);
        break;
    }
  }

  // Increment loop counter and report on stats if needed
  if (millis() > 10000 && debugLoopInfo) {
    arduinoLoopExecutionCount++;
    if (ptReportArduinoLoopStats.call()) {
      reportArduinoLoopRate(&arduinoLoopExecutionCount);
    }
  }
}
