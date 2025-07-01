#include <Arduino.h>
#include <ptScheduler.h>

#include "../shared/common_task_scheduling.h"
#include "shared/debug_logging.h"
#include "shared/ethernet/ethernet_receive_udp.h"
#include "shared/ethernet/ethernet_send_udp.h"

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

ptScheduler ptReportArduinoPerformanceStats = ptScheduler(PT_TIME_1MIN);

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Check if we need to execute any of the common scheduled tasks
void handleCommonScheduledTasks() {

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
