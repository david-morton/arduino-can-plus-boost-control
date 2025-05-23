/* ======================================================================
   COMMON VARIABLES: These are used across both master and slave Arduinos
   ====================================================================== */

extern bool clutchPressed;
extern bool inNeutral;
extern float afrBank1;
extern float afrBank2;
extern float crankCasePressureKpa;
extern float fuelPressureKpa;
extern float oilPressureKpa;
extern float oilTempCelcius;
extern float radiatorOutletTempCelcius;
extern float vehicleSpeedFront;
extern float vehicleSpeedRear;
extern int boostTargetKpa;
extern int brakeLinePressureFrontKpa;
extern int brakeLinePressureRearKpa;
extern int currentGear;
extern int engineSpeedRpm;
extern int engineTempCelcius;
extern int intakePressureBank1Kpa;
extern int intakePressureBank2Kpa;
extern int intakePressureManifoldKpa;
extern int intakeTempBank1Celcius;
extern int intakeTempBank2Celcius;
extern int intakeTempManifoldCelcius;
extern int radiatorFanDutyPercentage;


   # Slave Physical Connections - Mux
|  **Channel**  | **Use**                         | **Details**                                      |
|---------------|---------------------------------|--------------------------------------------------|
| C0            | Oil temperature                 | Voltage divider: 1.5k                            |
| C1            | Radiator outlet temperature     | Voltage divider: 1.5k                            |
| C2            | Intake temperature (Bank 1)     | Voltage divider: 1.2k                            |
| C3            | Intake temperature (Bank 2)     | Voltage divider: 1.2k                            |
| C4            | Intake temperature (Manifold)   | Voltage divider: 1.2k                            |
| C5            | Clutch status                   | Voltage divider: 3.3k + 10k + diode              |
| C6            | Neutral status                  | Voltage divider: 3.3k + 10k + diode              |
| C7            | Oil pressure                    | Direct analog input                              |
| C8            | Fuel pressure                   | Direct analog input                              |
| C9            | Crankcase vacuum/pressure       | Direct analog input                              |
| C10           | Front brake line pressure       | Direct analog input                              |
| C11           | Rear brake line pressure        | Direct analog input                              |


# Slave Physical Connections - Arduino
|  **Arduino Pin** | **Use**                      | **Details**                         |
|------------------|------------------------------|-------------------------------------|

| D2               | Tach signal                  | Interrupt-capable input             |
| D3               | Buzzer                       | Digital output                      |
| D4               | MUX select bit 0 (S0)        | MUX channel select                  |
| D5               | Boost solenoid 1             | PWM output                          |
| D6               | Boost solenoid 2             | PWM output                          |
| D7               | MUX select bit 1 (S1)        | MUX channel select                  |
| D8               | MUX select bit 2 (S2)        | MUX channel select                  |
| D9               | MUX select bit 3 (S3)        | MUX channel select                  |
| D10              | Ethernet shield CS           | SPI chip select                     |
| A0               | Intake pressure (Bank 1)     | Analog input                        |
| A1               | Intake pressure (Bank 2)     | Analog input                        |
| A2               | Intake pressure (Manifold)   | Analog input                        |
| A3               | MUX analog signal            | Output from MUX                     |
