#include <Arduino.h>

#include "../alarm/alarm_helpers.h"
#include "../debug_logging.h"
#include "../ethernet/ethernet_send_udp.h"
#include "../telemetry/telemetry_payload_ids.h"
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

  sendUdpMessageWithCommand(PAYLOAD_ID_SYSTEM_DATA, buffer);
}
