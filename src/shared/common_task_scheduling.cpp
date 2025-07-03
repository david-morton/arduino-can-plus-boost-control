#include <Arduino.h>
#include <ptScheduler.h>

#include "../shared/common_task_scheduling.h"
#include "shared/command_ids.h"
#include "shared/debug_logging.h"
#include "shared/ethernet/ethernet_ping_monitor.h"
#include "shared/ethernet/ethernet_receive_udp.h"
#include "shared/ethernet/ethernet_send_udp.h"
#include "shared/system_data/system_data_send.h"
#include "shared/telemetry/telemetry_send_staging.h"

#ifdef ARDUINO_MASTER
#include "../master/can/can_helpers.h"
#endif

/* ======================================================================
   VARIABLES
   ====================================================================== */

unsigned long arduinoLoopExecutionCount = 0;

/* ======================================================================
   OBJECTS: Pretty tiny scheduler objects / tasks
   ====================================================================== */

// High frequency tasks (tens of milliseconds)

// Medium frequency tasks (hundreds of milliseconds)
ptScheduler ptSendDataStateToRemote = ptScheduler(PT_TIME_200MS);

// Low frequency tasks (seconds)
ptScheduler ptReportArduinoPerformanceStats  = ptScheduler(PT_TIME_1MIN);
ptScheduler ptHandlePingTimeoutsAndLoss      = ptScheduler(PT_TIME_10S);
ptScheduler ptSendPingRequestToRemoteArduino = ptScheduler(PT_TIME_1S);

// Send different message classes to remote Arduino
ptScheduler ptSendLowFrequencyMessages    = ptScheduler(PT_TIME_1S);
ptScheduler ptSendMediumFrequencyMessages = ptScheduler(PT_TIME_100MS);
ptScheduler ptSendHighFrequencyMessages   = ptScheduler(PT_TIME_20MS);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

void handleCommonScheduledTasks() {
  // Send shared system data state to remote Arduino
  if (ptSendDataStateToRemote.call()) {
    sendDataStateToRemote();
  }

  // Send low frequency messages
  if (ptSendLowFrequencyMessages.call()) {
    sendStagedTelemetry(MSG_MASTER_LOW_FREQUENCY, CMD_LOW_FREQUENCY_MESSAGES);
  }

  // Send medium frequency messages
  if (ptSendMediumFrequencyMessages.call()) {
    sendStagedTelemetry(MSG_MASTER_MED_FREQUENCY, CMD_MED_FREQUENCY_MESSAGES);
  }

  // Send high frequency messages
  if (ptSendHighFrequencyMessages.call()) {
    sendStagedTelemetry(MSG_MASTER_HIGH_FREQUENCY, CMD_HIGH_FREQUENCY_MESSAGES);
  }

  // Report ping RTT stats (if needed) from buffer average
  if (ptHandlePingTimeoutsAndLoss.call()) {
    handlePingTimeoutsAndLoss();
  }

  // Issue ping request to remote Arduino
  if (ptSendPingRequestToRemoteArduino.call()) {
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
