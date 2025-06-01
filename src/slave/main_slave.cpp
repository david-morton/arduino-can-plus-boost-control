#include <Arduino.h>
#include <ptScheduler.h> // The task scheduling library of choice

#include "shared/common/command_ids.h"
#include "shared/common/ethernet/ethernet_helpers.h"
#include "shared/common/ethernet/ethernet_ping.h"
#include "shared/common/ethernet/ethernet_receive.h"
#include "shared/common/ethernet/ethernet_send.h"
#include "shared/common/helpers_logging.h"
#include "shared/common/message_handlers_udp.h"
#include "shared/common/variables.h"
#include "shared/light_sensor/light_sensor.h"

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
bool debugGeneral         = true;
bool debugLightSensor     = true;
bool debugPerformance     = false;

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
char udpReceiveBuffer[RECEIVE_PACKET_BUFFER_SIZE];

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */

unsigned long arduinoLoopExecutionCount = 0;
int           currentLuxReading         = 0; // Variable to store the current lux reading from ambient light sensor

/* ======================================================================
   OBJECTS: Pretty tiny scheduler objects / tasks
   ====================================================================== */
// High frequency tasks

// Medium frequency tasks

// Low frequency tasks
ptScheduler ptReportArduinoLoopStats   = ptScheduler(PT_TIME_5S);
ptScheduler ptGetAmbientLightReading   = ptScheduler(PT_TIME_2S);
ptScheduler ptSendLowFrequencyMessages = ptScheduler(PT_TIME_1S);

/* ======================================================================
   SETUP
   ====================================================================== */
void setup() {
  Serial.begin(500000); // Use a high baud rate for faster serial communication

  DEBUG_GENERAL("INFO: Entering main setup phase ...");

  initialiseEthernetShield(ethConfigLocal);
  initialiseAmbientLightSensor();
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

  // Read ambient light sensor value at a defined interval
  if (ptGetAmbientLightReading.call()) {
    currentLuxReading = getAverageLux();
  }

  // Send low frequency messages at a defined interval
  if (ptSendLowFrequencyMessages.call()) {
    // Format message as: "2,<luxValue>"
    char messageBuffer[24]; // Enough for command + comma + signed int
    snprintf(messageBuffer, sizeof(messageBuffer), "%d,%d", CMD_LOW_FREQUENCY_MESSAGES, currentLuxReading);

    DEBUG_ETHERNET_TRAFFIC("Sending lux message: %s", messageBuffer);
    sendUdpMessage(messageBuffer);
  }

  // Increment loop counter and report on stats if needed
  if (millis() > 10000 && debugPerformance) {
    arduinoLoopExecutionCount++;
    if (ptReportArduinoLoopStats.call()) {
      reportArduinoLoopRate(&arduinoLoopExecutionCount);
    }
  }
}
