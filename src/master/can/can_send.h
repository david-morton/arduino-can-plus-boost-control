#pragma once

/* ======================================================================
   FUNCTION PROTOTYPES
   ====================================================================== */

void sendCanMessages();
void sendEngineRpmToBmwCan();
void sendEngineTempToBmwCan();
void sendMiscToBmwCan();
void sendVehicleSpeedToNissanCan();
