#include <Arduino.h>
#include <mcp2515_can.h> // Used for CAN shields
#include <ptScheduler.h> // The task scheduling library of choice

#include "shared/common/ethernet_helpers.h"
#include "shared/common/general_helpers.h"
#include "shared/common/variables.h"
#include "shared/mqtt/mqtt_helpers.h"

#define CAN_2515

/* ======================================================================
   VARIABLES: Major functional area toggles
   ====================================================================== */

/* ======================================================================
   VARIABLES: Debug and stat output
   ====================================================================== */
bool debugEthernetGeneral = true;
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
    .mac = {0xA8, 0x61, 0x0A, 0xAE, 0x23, 0x6E},
    .ip  = IPAddress(192, 168, 10, 100)};

// Define remote IP address for peer Arduino native messeging
const IPAddress remoteArduinoIp(192, 168, 10, 101);

// Define UDP receive buffer
char udpReceiveBuffer[RECEIVE_PACKET_BUFFER_SIZE];

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */

unsigned long arduinoLoopExecutionCount = 0;

/* ======================================================================
   OBJECTS: Pretty tiny scheduler objects / tasks
   ====================================================================== */
// High frequency tasks

// Medium frequency tasks
ptScheduler ptSendPingRequestToRemoteArduino = ptScheduler(PT_TIME_50MS);
ptScheduler ptCheckForEthernetMessages       = ptScheduler(PT_TIME_100MS);

// Low frequency tasks
ptScheduler ptReportArduinoLoopStats = ptScheduler(PT_TIME_5S);
ptScheduler ptReportArduinoPingStats = ptScheduler(PT_TIME_5S);

/* ======================================================================
   SETUP
   ====================================================================== */
void setup() {
  Serial.begin(1000000); // Use a high baud rate for faster serial communication

  DEBUG_GENERAL("INFO: Entering main setup phase ...");

  initialiseEthernetShield(ethConfigLocal);
  connectMqttClientToBroker();
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
    // Extract payload pointer — the remainder of the message after the command ID
    char *payloadStr = strtok(nullptr, "");

    int commandId = atoi(commandIdStr);

    // Dispatch by command ID
    switch (commandId) {
      case 0: { // Ping request
        // Extract sender's sendSequenceNumber used as ping identifier
        char *pingSeqStr = strtok(nullptr, ",");
        sendPingResponseToRemoteArduino(atoi(pingSeqStr));
        break;
      }
      case 1: { // Ping response, extract ping sequence number from payload and handle it
        unsigned long pingSeq = strtoul(payloadStr, nullptr, 10);
        handlePingResponse(micros(), pingSeq); // Your handler
        break;
      }

      default:
        DEBUG_GENERAL("Unknown command ID: %d", commandId);
        break;
    }
  }

  // Report ping RTT stats if needed from buffer average
  if (ptReportArduinoPingStats.call()) {
    DEBUG_ETHERNET_GENERAL("Average ping RTT: %.2f ms", getAveragePingRtt() / 1000.0);
  }

  // Issue ping request to remote Arduino
  if (ptSendPingRequestToRemoteArduino.call()) {
    sendArduinoPingRequest();
  }

  // Increment loop counter and report on stats if needed
  if (millis() > 10000 && debugLoopInfo) {
    arduinoLoopExecutionCount++;
    if (ptReportArduinoLoopStats.call()) {
      reportArduinoLoopRate(&arduinoLoopExecutionCount);
    }
  }
}
