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

int  setupRetriesMax  = 5;     // Maximum number of retries for initialising CAN modules
volatile bool canBmwMsgRecv    = false; // Flag to indicate if a message has been received on the BMW CAN bus
volatile bool canNissanMsgRecv = false; // Flag to indicate if a message has been received on the Nissan CAN bus

/* ======================================================================
   ISRs: Set flags when interrupts are triggered
   ====================================================================== */

// Set the flag to indicate that a message has been received on the BMW CAN bus
void setCanBmwMessageReceivedFlag() {
  canBmwMsgRecv = true;
}

// Set the flag to indicate that a message has been received on the Nissan CAN bus
void setCanNissanMessageReceivedFlag() {
  canNissanMsgRecv = true;
}

/* ======================================================================
   FUNCTION DEFINITIONS
   ====================================================================== */

// Initialise both CAN modules (BMW and Nissan)
void initialiseCanModules() {
  DEBUG_GENERAL("Initialising CAN modules ...");
  initialiseCanModule(CAN_BMW, "BMW");
  initialiseCanModule(CAN_NISSAN, "NISSAN");
  configureCanMasksAndFilters();
  configureCanInterrupts();
}

// Initialise the CAN modules for BMW and Nissan CAN buses
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

// Configure masks and filters for shields to reduce noise. There are two masks in the mcp2515 which both need to be
// set. Mask 0 has 2 filters and mask 1 has 4 so we set them all as needed.
// 0x551 is where coolant temperature is located and 0x7E8 is for results of queried CAN parameters.
void configureCanMasksAndFilters() {
  DEBUG_GENERAL("Configuring CAN masks and filters ...");
  CAN_NISSAN.init_Mask(0, 0, 0xFFF);
  CAN_NISSAN.init_Filt(0, 0, 0x551);
  CAN_NISSAN.init_Filt(1, 0, 0x7E8);

  CAN_NISSAN.init_Mask(1, 0, 0xFFF);
  CAN_NISSAN.init_Filt(2, 0, 0x551);
  CAN_NISSAN.init_Filt(3, 0, 0x7E8);
  CAN_NISSAN.init_Filt(4, 0, 0x551);
  CAN_NISSAN.init_Filt(5, 0, 0x7E8);

  // 0x1F0 is where the individual wheel speeds are
  // https://www.bimmerforums.com/forum/showthread.php?1887229-E46-Can-bus-project
  CAN_BMW.init_Mask(0, 0, 0xFFF);
  CAN_BMW.init_Filt(0, 0, 0x1F0);
  CAN_BMW.init_Filt(1, 0, 0x1F0);

  CAN_BMW.init_Mask(1, 0, 0xFFF);
  CAN_BMW.init_Filt(2, 0, 0x1F0);
  CAN_BMW.init_Filt(3, 0, 0x1F0);
  CAN_BMW.init_Filt(4, 0, 0x1F0);
  CAN_BMW.init_Filt(5, 0, 0x1F0);
}

// Configure interrupts for the CAN modules
void configureCanInterrupts() {
  DEBUG_GENERAL("Configuring CAN interrupts ...");
  attachInterrupt(digitalPinToInterrupt(CAN_INTERRUPT_PIN_BMW), setCanBmwMessageReceivedFlag, FALLING);
  attachInterrupt(digitalPinToInterrupt(CAN_INTERRUPT_PIN_NISSAN), setCanNissanMessageReceivedFlag, FALLING);
}
