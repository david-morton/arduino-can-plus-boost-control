#include <Arduino.h>
#include <ptScheduler.h> // The task scheduling library of choice

#include "shared/common/ethernet_helpers.h"
#include "shared/common/general_helpers.h"

#define CAN_2515

/* ======================================================================
   VARIABLES: Major functional area toggles
   ====================================================================== */

/* ======================================================================
   VARIABLES: Debug and stat output
   ====================================================================== */
bool debugEthernetReceive = true;
bool debugEthernetSend    = true;
bool debugGears           = true;
bool debugGeneral         = true;
bool debugLoopInfo        = true;

bool reportArduinoLoopStats = true;

/* ======================================================================
   VARIABLES: More words
   ====================================================================== */

EthernetConfig ethConfig = {
    .mac = {0xA8, 0x61, 0x0A, 0xAE, 0x1F, 0xF4},
    .ip  = IPAddress(192, 168, 10, 101)};

/* ======================================================================
   VARIABLES: General use / functional
   ====================================================================== */

unsigned long arduinoLoopExecutionCount = 0;

/* ======================================================================
   OBJECTS: Pretty tiny scheduler objects / tasks
   ====================================================================== */
// High frequency tasks

// Medium frequency tasks

// Low frequency tasks
ptScheduler ptReportArduinoLoopStats = ptScheduler(PT_TIME_5S);

/* ======================================================================
   SETUP
   ====================================================================== */
void setup() {
  Serial.begin(115200);
  while (!Serial) {
  };

  Serial.println("INFO: Entering main setup phase ...\n");

  initialiseEthernetShield(ethConfig);
}

/* ======================================================================
   MAIN LOOP
   ====================================================================== */
void loop() {
  // Increment loop counter if needed so we can report on stats
  if (millis() > 10000 && reportArduinoLoopStats) {
    arduinoLoopExecutionCount++;
    if (ptReportArduinoLoopStats.call()) {
      reportArduinoLoopRate(&arduinoLoopExecutionCount);
    }
  }
}
