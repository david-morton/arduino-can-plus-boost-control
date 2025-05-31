#include <Arduino.h>
#include <mcp2515_can.h> // Used for CAN shields
#include <ptScheduler.h> // The task scheduling library of choice

#include "shared/common/command_ids.h"
#include "shared/common/ethernet/ethernet_helpers.h"
#include "shared/common/ethernet/ethernet_ping.h"
#include "shared/common/ethernet/ethernet_receive.h"
#include "shared/common/ethernet/ethernet_send.h"
#include "shared/common/helpers_logging.h"
#include "shared/common/message_handlers_udp.h"
#include "shared/common/variables.h"
#include "shared/mqtt/mqtt_helpers.h"

#define CAN_2515

/* ======================================================================
   VARIABLES: Major functional area toggles
   ====================================================================== */

/* ======================================================================
   VARIABLES: Debug and stat output
   ====================================================================== */
bool debugError           = true;
bool debugEthernetGeneral = false;
bool debugEthernetTraffic = false;
bool debugEthernetPing    = false;
bool debugGears           = false;
bool debugGeneral         = false;
bool debugPerformance     = false;

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

// Low frequency tasks
ptScheduler ptReportArduinoLoopStats         = ptScheduler(PT_TIME_5S);
ptScheduler ptReportArduinoPingStats         = ptScheduler(PT_TIME_5S);
ptScheduler ptSendPingRequestToRemoteArduino = ptScheduler(PT_TIME_1S);

/* ======================================================================
   SETUP
   ====================================================================== */
void setup() {
  Serial.begin(500000); // Use a high baud rate for faster serial communication

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
    // Extract command ID from the received UDP message
    char *commandIdStr = strtok(udpReceiveBuffer, ",");
    if (!commandIdStr) {
      DEBUG_ERROR("Malformed UDP payload: missing command ID");
      return;
    }
    // Extract payload pointer — the remainder of the message after the command ID
    char *payloadStr = strtok(nullptr, "");

    // Convert command ID string to integer
    int commandId = atoi(commandIdStr);

    // Dispatch by command ID
    switch (commandId) {
      case CMD_RECEIVE_PING_REQUEST: {
        handlePingRequestOrResponse(0, payloadStr, strlen(payloadStr));
        break;
      }
      case CMD_RECEIVE_PING_RESPONSE: {
        handlePingRequestOrResponse(1, payloadStr, strlen(payloadStr));
        break;
      }
      default:
        DEBUG_ERROR("Unknown command ID: %d", commandId);
        break;
    }
  }

  // Report ping RTT stats if needed from buffer average
  if (ptReportArduinoPingStats.call()) {
    checkPingTimeoutsAndLoss();
  }

  // Issue ping request to remote Arduino
  if (ptSendPingRequestToRemoteArduino.call()) {
    sendArduinoPingRequest();
  }

  // Increment loop counter and report on stats if needed
  if (millis() > 10000 && debugPerformance) {
    arduinoLoopExecutionCount++;
    if (ptReportArduinoLoopStats.call()) {
      reportArduinoLoopRate(&arduinoLoopExecutionCount);
    }
  }
}
