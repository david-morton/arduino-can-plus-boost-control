# NOTE: This project is WIP ... readme content to follow

# Project Purpose & Description
- This project contains code for two Arduino R4's. The 'master' lives in the rear of the vehicle and is essentially the brains of the solution. The 'slave' is in the front of the vehicle and is the muscle, handling sensor readings and controlling boost solenoids.

# Hardware Used
Below is a complete breakdown of the hardware used for this solution. Refer to the embedded diagram for more detail.
- 2x Arduino R4 WiFi
- 2x Arduino Ethernet shield V2's
- 2x Joy-IT CAN breakout boards (MCP2515 + TJA1050)
- Ethernet switch
- MAC <part number> 3 port solenoid valves

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
| C12           |                                 |                                                  |
| C13           |                                 |                                                  |
| C14           |                                 |                                                  |
| C15           |                                 |                                                  |

# Slave Physical Connections - Arduino
## 🔌 Physical Pin Connections (Arduino Uno R4 WiFi) — Ordered by Pin Number

|  **Arduino Pin** | **Use**                      | **Details**                         |
|------------------|------------------------------|-------------------------------------|
| D0               | RESERVED                     | Arduino serial                      |
| D1               | RESERVED                     | Arduino serial                      |
| D2               | Tach signal                  | Interrupt-capable input             |
| D3               | Buzzer                       | Digital output                      |
| D4               | MUX select bit 0 (S0)        |                                     |
| D5               | Boost solenoid 1             | PWM output                          |
| D6               | Boost solenoid 2             | PWM output                          |
| D7               | MUX select bit 1 (S1)        | MUX channel select                  |
| D8               | MUX select bit 2 (S2)        | MUX channel select                  |
| D9               | MUX select bit 3 (S3)        | MUX channel select                  |
| D10              | Ethernet shield CS           | SPI chip select                     |
| D11              | RESERVED                     | Arduino SPI MOSI                    |
| D12              | RESERVED                     | Arduino SPI MISO                    |
| D13              | RESERVED                     | Arduino SPI SCK                     |
| A0               | Intake pressure (Bank 1)     | Analog input                        |
| A1               | Intake pressure (Bank 2)     | Analog input                        |
| A2               | Intake pressure (Manifold)   | Analog input                        |
| A3               | MUX analog signal            | Output from MUX                     |
| A4               | I2C (SDA)                    | PWM driver & ambient light modules  |
| A5               | I2C (SCL)                    | PWM driver & ambient light modules  |

# Implementation Detail & Architecture
- Words here later

# Communications
Communication between Arduino's and the Orange Pi is over Ethernet. The master and slave Arduinos use UDP, and a custom message format defined below. The Arduino's process messages from the buffer in a FIFO manner.

According to my research the Ethernet buffer should only ever contain whole messages. If a message is unable to be accepted in full, it is discarded.

### Message Structures
- Message boundaries are indicated by < and > symbols for start and end respectively
- We cater for multiple fields by being comma delimited
- A basic XOR checksum is used so we can detect and discard corrupt messages

### Message Types
An overview of the message types and formats can be found below:
- Request current readings from slave such as current pressures, temperatures and error status
- Request current readings for speed, RPM, gear and clutch pedal status from master to this boost controller
- Respond to the masters request for info from boost controller (this code base)

### Example Messages
The standard message format is as below. The number and type of the data fields is unique to each commandId.
<sequenceNumber,commandId,data1,data2,data3,...,checksum>

Command ID's in use are as below. The master is the 'main' car module which drives the dashboard via CAN and the slave is the boost controller who's code is in this repo.
| Command ID  | Direction       | Data Fields                                                     | Description                                       |
| ----------- | --------------- | --------------------------------------------------------------- | ------------------------------------------------- |
| 0           | Master to slave | salt (allows for checksum calculation)                          | Request current data from boost controller        |
| 1           | Master to slave | currentRpm,currentSpeed,currentGear,clutchPressed               | Push current data from master to boost controller |
| 2           | Slave to master | errorStatus,currentBoost,currentTemp,currentValveOpenPercentage | Response to command ID 0                          |

# Technical Notes

# Boost Control Rules / Behaviour
The following conditions will set a lower than maximum boost target
- Clutch pressed
- Car is in neutral
- Combinations of gear, speed and RPM

The following conditions cause a failsafe condition (as little boost as possible)
- Critical fault detected
  - No ethernet comms from master
  - Overboost condition (duration and pressure delta)
