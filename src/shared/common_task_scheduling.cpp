#include <Arduino.h>
#include <ptScheduler.h>

#include "../shared/common_task_scheduling.h"
#include "shared/debug_logging.h"
#include "shared/ethernet/ethernet_helpers.h"
#include "shared/ethernet/ethernet_ping_monitor.h"
#include "shared/ethernet/ethernet_receive_udp.h"
#include "shared/ethernet/ethernet_send_udp.h"
#include "shared/system_data/system_data_send.h"
#include "shared/telemetry/telemetry_payload_ids.h"
#include "shared/telemetry/telemetry_send_staging.h"

#ifdef ARDUINO_MASTER
#include "../master/can/can_helpers.h"
#endif

/* ======================================================================
   VARIABLES
   ====================================================================== */

unsigned long arduinoLoopExecutionCount = 0;

/* ======================================================================
   SCHEDULER OBJECTS
   ====================================================================== */

// High frequency tasks (tens of milliseconds)

// Medium frequency tasks (hundreds of milliseconds)
ptScheduler ptSendDataStateToRemote = ptScheduler(PT_TIME_200MS);

// Low frequency tasks (seconds)
ptScheduler ptHandlePingTimeoutsAndLoss      = ptScheduler(PT_TIME_10S);
ptScheduler ptReportArduinoPerformanceStats  = ptScheduler(PT_TIME_1MIN);
ptScheduler ptSendPingRequestToRemoteArduino = ptScheduler(PT_TIME_1S);
ptScheduler ptUpdateEthernetLinkStatus       = ptScheduler(PT_TIME_1S);

// Send different message classes to remote Arduino
ptScheduler ptSendLowFrequencyMessages    = ptScheduler(PT_TIME_1S);
ptScheduler ptSendMediumFrequencyMessages = ptScheduler(PT_TIME_100MS);
ptScheduler ptSendHighFrequencyMessages   = ptScheduler(PT_TIME_20MS);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

void handleCommonScheduledTasks() {
  // Update the current lux reading from the remote Arduino
  if (ptUpdateEthernetLinkStatus.call()) {
    updateEthernetLinkStatus();
  }

  // Send shared system data state to remote Arduino if peer is online
  if (globalHealthEthernetConnected && globalHealthEthernetPeerOnline && ptSendDataStateToRemote.call()) {
    sendDataStateToRemote();
  }

  // Send low frequency messages if peer is online
  if (globalHealthEthernetConnected && globalHealthEthernetPeerOnline && ptSendLowFrequencyMessages.call()) {
    sendStagedTelemetry(TELEMETRY_CLASS_LOW, PAYLOAD_ID_LOW_FREQUENCY_MESSAGES);
  }

  // Send medium frequency messages if peer is online
  if (globalHealthEthernetConnected && globalHealthEthernetPeerOnline && ptSendMediumFrequencyMessages.call()) {
    sendStagedTelemetry(TELEMETRY_CLASS_MED, PAYLOAD_ID_MED_FREQUENCY_MESSAGES);
  }

  // Send high frequency messages if peer is online
  if (globalHealthEthernetConnected && globalHealthEthernetPeerOnline && ptSendHighFrequencyMessages.call()) {
    sendStagedTelemetry(TELEMETRY_CLASS_HIGH, PAYLOAD_ID_HIGH_FREQUENCY_MESSAGES);
  }

  // Report ping RTT stats (if needed) from buffer average
  if (globalHealthEthernetConnected && ptHandlePingTimeoutsAndLoss.call()) {
    handlePingTimeoutsAndLoss();
  }

  // Issue ping request to remote Arduino if Ethernet link is up
  if (globalHealthEthernetConnected && ptSendPingRequestToRemoteArduino.call()) {
    sendArduinoPingRequest();
  }

  // Update and output loop rate if needed
  if (debugPerformance && millis() > 10000) {
    arduinoLoopExecutionCount++;
    if (ptReportArduinoPerformanceStats.call()) {
      reportArduinoLoopRate(&arduinoLoopExecutionCount);
      reportUdpMessageSendStats();
      reportUdpMessageReceiveStats();
#ifdef ARDUINO_MASTER
      reportReceiveCanMessageRate();
#endif
    }
  }
}
