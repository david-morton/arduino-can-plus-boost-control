#include <Arduino.h>

#include "../alarm/alarm_helpers.h"
#include "../command_ids.h"
#include "../debug_logging.h"
#include "../ethernet/ethernet_send_udp.h"
#include "system_data_send.h"

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Send the shared system data state over UDP
void sendDataStateToRemote() {
  char buffer[64]; // Adjust if adding more fields later

  int written = snprintf(buffer, sizeof(buffer), "warn=%d,crit=%d", localAlarmWarningState, localAlarmCriticalState);

  if (written < 0 || written >= (int)sizeof(buffer)) {
    DEBUG_ERROR("Shared system data buffer overrun!");
    return;
  }

  sendUdpMessageWithCommand(CMD_SYSTEM_DATA, buffer);
}

// [00:00:34.001] [DEBUG ETHERNET MESSAGES] Sent UDP message with command ID 5: 5,warn=0,crit=0
// [00:00:34.003] [DEBUG ERROR] Global Alarm States: Critical: ON, Warning: OFF