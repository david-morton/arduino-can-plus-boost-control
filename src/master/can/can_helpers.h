#pragma once

#include <Arduino.h>
#include <mcp2515_can.h> // Used for MCP2515 CAN modules

/* ======================================================================
   OBJECTS
   ====================================================================== */

extern mcp2515_can CAN_BMW;
extern mcp2515_can CAN_NISSAN;

/* ======================================================================
   VARIABLES
   ====================================================================== */

extern volatile bool canBmwMsgRecv;
extern volatile bool canNissanMsgRecv;

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void initialiseCanModule(mcp2515_can &canModule, const char *label);
void initialiseCanModules();
void configureCanMasksAndFilters();
void configureCanInterrupts();
