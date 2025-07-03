#include <Arduino.h>
#include <mcp2515_can.h> // Used for MCP2515 CAN modules

#include "../../shared/debug_logging.h"
#include "../pin_assignments_master.h"
#include "can_helpers.h"

/* ======================================================================
   OBJECTS
   ====================================================================== */

mcp2515_can CAN_BMW(CAN_SPI_CS_PIN_BMW);
mcp2515_can CAN_NISSAN(CAN_SPI_CS_PIN_NISSAN);

/* ======================================================================
   VARIABLES
   ====================================================================== */

int           setupRetriesMax                  = 5; // Maximum number of retries for initialising CAN modules
unsigned long receiveCanRateMessageCountBmw    = 0;
unsigned long receiveCanRateMessageCountNissan = 0;
unsigned long receiveCanRateLasttimestamp      = millis();

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Update the total number of received CAN messages
void updateReceiveCanMessageCountBmw() {
  receiveCanRateMessageCountBmw++;
}

void updateReceiveCanMessageCountNissan() {
  receiveCanRateMessageCountNissan++;
}

// Output CAN message rate (msgs/sec) since last call
void reportReceiveCanMessageRate() {
  unsigned long currentTime = millis();
  unsigned long elapsedMs   = currentTime - receiveCanRateLasttimestamp;

  if (elapsedMs >= 1000) {
    unsigned long rateBmw    = (elapsedMs > 0) ? ((receiveCanRateMessageCountBmw * 1000UL) / elapsedMs) : 0;
    unsigned long rateNissan = (elapsedMs > 0) ? ((receiveCanRateMessageCountNissan * 1000UL) / elapsedMs) : 0;

    DEBUG_PERFORMANCE("CAN message processing rate BMW:    %lu msg/s", rateBmw);
    DEBUG_PERFORMANCE("CAN message processing rate Nissan: %lu msg/s", rateNissan);

    receiveCanRateLasttimestamp      = currentTime;
    receiveCanRateMessageCountBmw    = 0;
    receiveCanRateMessageCountNissan = 0;
  }
}

// Initialise both CAN modules (BMW and Nissan)
void initialiseCanModules() {
  DEBUG_GENERAL("Initialising CAN modules ...");
  initialiseCanModule(CAN_BMW, "BMW");
  initialiseCanModule(CAN_NISSAN, "NISSAN");
  configureCanMasksAndFilters();
}

// Initialise a single CAN module
void initialiseCanModule(mcp2515_can &canModule, const char *label) {
  for (int i = 0; i < setupRetriesMax; i++) {
    bool result = canModule.begin(CAN_500KBPS);
    if (result == CAN_OK) {
      DEBUG_GENERAL("\t\t%s CAN module initialised successfully", label);
      return;
    } else {
      DEBUG_ERROR("\t\t%s CAN module init failed, retrying ...", label);
      delay(250);
    }
  }

  DEBUG_ERROR("\t\t%s CAN module init failed after maximum retries", label);
}

// Configure CAN masks and filters for both buses
void configureCanMasksAndFilters() {
  DEBUG_GENERAL("\t\tConfiguring CAN masks and filters ...");

  // Nissan (0x551 = coolant temp, 0x7E8 = OBD-II response)
  CAN_NISSAN.init_Mask(0, 0, 0xFFF);
  CAN_NISSAN.init_Filt(0, 0, 0x551);
  CAN_NISSAN.init_Filt(1, 0, 0x7E8);

  CAN_NISSAN.init_Mask(1, 0, 0xFFF);
  CAN_NISSAN.init_Filt(2, 0, 0x551);
  CAN_NISSAN.init_Filt(3, 0, 0x7E8);
  CAN_NISSAN.init_Filt(4, 0, 0x551);
  CAN_NISSAN.init_Filt(5, 0, 0x7E8);

  // BMW (0x1F0 = wheel speeds)
  CAN_BMW.init_Mask(0, 0, 0xFFF);
  CAN_BMW.init_Filt(0, 0, 0x1F0);
  CAN_BMW.init_Filt(1, 0, 0x1F0);

  CAN_BMW.init_Mask(1, 0, 0xFFF);
  CAN_BMW.init_Filt(2, 0, 0x1F0);
  CAN_BMW.init_Filt(3, 0, 0x1F0);
  CAN_BMW.init_Filt(4, 0, 0x1F0);
  CAN_BMW.init_Filt(5, 0, 0x1F0);
}
