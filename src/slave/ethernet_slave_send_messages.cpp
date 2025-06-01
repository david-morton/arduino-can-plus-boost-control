#include <Arduino.h>

#include "../shared/common/command_ids.h"
#include "../shared/common/ethernet/ethernet_helpers.h"
#include "../shared/common/ethernet/ethernet_send.h"
#include "../shared/common/helpers_logging.h"
#include "../shared/common/variables.h"

// Function to send low frequency telemetry in format CMD,<lux>,<...future values>
void sendLowFrequencyTelemetry() {
  char messageBuffer[64]; // Adjust size as fields are added
  snprintf(messageBuffer, sizeof(messageBuffer), "%d,%d", CMD_LOW_FREQUENCY_MESSAGES, currentLuxReading);
  DEBUG_ETHERNET_TRAFFIC("Sending low frequency telemetry: %s", messageBuffer);
  sendUdpMessage(messageBuffer);
}
