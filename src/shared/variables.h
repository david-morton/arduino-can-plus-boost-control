#include <Ethernet.h>
#include <EthernetUdp.h>

#include "Arduino.h"

/* ======================================================================
   COMMON VARIABLES: These are used across both master and slave Arduinos
   ====================================================================== */

// Measured sensor variables
extern bool  clutchPressed;
extern bool  inNeutral;
extern float afrBank1;
extern float afrBank2;
extern float crankCasePressureKpa;
extern float fuelPressureKpa;
extern float oilPressureKpa;
extern float oilTempCelcius;
extern float radiatorOutletTempCelcius;
extern float vehicleSpeedFront;
extern float vehicleSpeedRear;
extern int   brakeLinePressureFrontKpa;
extern int   brakeLinePressureRearKpa;
extern int   currentElectronicsTemp;
extern int   currentLuxReading;
extern int   engineSpeedRpm;
extern int   engineTempCelcius;
extern int   intakePressureBank1Kpa;
extern int   intakePressureBank2Kpa;
extern int   intakePressureManifoldKpa;
extern int   intakeTempBank1Celcius;
extern int   intakeTempBank2Celcius;
extern int   intakeTempManifoldCelcius;

// Defined or derived variables
extern bool            remoteArduinoResponding;
extern int             boostTargetKpa;
extern int             currentGear;
extern int             radiatorFanDutyPercentage;
extern const IPAddress remoteArduinoIp;
extern const uint16_t  localArduinoListenPort;

// General use variables
extern unsigned long sendSequenceNumber;

// Clients
extern EthernetClient ethClient;
extern EthernetUDP    ethUdpClient;
