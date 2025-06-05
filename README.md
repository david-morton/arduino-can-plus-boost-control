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
|  **Arduino Pin** | **Use**                      | **Details**                         |
|------------------|------------------------------|-------------------------------------|
| D0               | RESERVED                     | Arduino serial                      |
| D1               | RESERVED                     | Arduino serial                      |
| D2               | Tach signal                  | Interrupt-capable input             |
| D3               | Buzzer                       | Digital output                      |
| D4               | MUX select bit 0 (S0)        | MUX channel select                  |
| D5               | Boost solenoid 1             | PWM output                          |
| D6               | Boost solenoid 2             | PWM output                          |
| D7               | MUX select bit 1 (S1)        | MUX channel select                  |
| D8               | MUX select bit 2 (S2)        | MUX channel select                  |
| D9               | MUX select bit 3 (S3)        | MUX channel select                  |
| D10              | Ethernet shield CS           | SPI chip select                     |
| D11              | RESERVED                     | Arduino SPI (MOSI)                  |
| D12              | RESERVED                     | Arduino SPI (MISO)                  |
| D13              | RESERVED                     | Arduino SPI (SCK)                   |
| A0               | Intake pressure (Bank 1)     | Analog input                        |
| A1               | Intake pressure (Bank 2)     | Analog input                        |
| A2               | Intake pressure (Manifold)   | Analog input                        |
| A3               | MUX analog signal            | Output from MUX                     |
| A4               | RESERVED                     | Arduino I2C (SDA)                   |
| A5               | RESERVED                     | Arduino I2C (SCL)                   |

# Implementation Detail & Architecture
- Words here later

# Communications
Communication between Arduino's and the Orange Pi is over Ethernet. The master and slave Arduinos use UDP, and a custom message format defined below. The Arduino's process messages from internal 'staging' buffers to allow decoupled send, receive and usage.

The Ethernet buffer should only ever contain whole messages. If a message is unable to be accepted in full, it is discarded. We do include a checksum in our payloads, but do not implement any ack or retry.

### Message Structures
- We cater for multiple fields by being comma delimited
- A basic XOR checksum is used so we can detect and discard corrupt messages
- A sequence number is implemented for debug and checksum salting, of sorts

### Message Types
An overview of the message types and formats can be found below:
- Request current readings from slave such as current pressures, temperatures and error status
- Request current readings for speed, RPM, gear and clutch pedal status from master to this boost controller
- Respond to the masters request for info from boost controller (this code base)

### Example Messages
The standard message format is as below. The number and type of the data fields is unique to each commandId.
<sequenceNumber,commandId,key1=value1,key2=value2,key3=value3,...,checksum>

Command ID's in use are as below. The master is the 'main' car module which drives the dashboard via CAN and the slave is the boost controller who's code is in this repo.
| Command ID  | Direction       | Data Fields                                                     | Description                                       |
| ----------- | --------------- | --------------------------------------------------------------- | ------------------------------------------------- |
| 0           | Both            | Nil                                                             | Ping request                                      |
| 1           | Both            | Nil                                                             | Ping response                                     |
| 2           | Slave to master | currentLuxReading                                               | Send low frequency messages                       |
| x           | Master to slave | currentRpm,currentSpeed,currentGear,clutchPressed               | Push current data from master to boost controller |
| x           | Slave to master | errorStatus,currentBoost,currentTemp,currentValveOpenPercentage | Response to command ID 0                          |

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

## 🧠 Function Naming Standards

To ensure clarity and consistency across all modules in this project (master/slave boards, shared code, telemetry), we follow a strict naming convention for functions and scheduler tasks. These prefixes describe the **intent and behavior** of each function, enabling fast comprehension and reliable maintenance.

### ✅ Canonical Prefixes

| **Prefix**   | **Used For**                                                                 | **Examples**                                 |
|--------------|-------------------------------------------------------------------------------|----------------------------------------------|
| `get`        | Accessing a previously stored or computed value (no I/O or mutation)         | `getLatestCoolantTemp()`, `getMuxState()`    |
| `read`       | Reading directly from a sensor or hardware interface                         | `readLuxSensor()`, `readMuxChannel()`        |
| `update`     | Refreshing internal state from sensors, events, or calculations              | `updateSensorStaging()`, `updatePingStatus()`|
| `send`       | Transmitting data out of the device (e.g. UDP, Serial)                       | `sendStagedTelemetry()`, `sendPingRequest()` |
| `process`    | Multi-step transformation or decision logic (routing, validation, storing)   | `processReceivedTelemetry()`, `processCommand()` |
| `calculate`  | Mathematical operations with no side effects                                 | `calculateAFR()`, `calculateTargetBoost()`   |
| `handle`     | Event-driven handlers (commands, interrupts, responses)                      | `handleCommand()`, `handlePingResponse()`    |
| `build`      | Constructing structured data for output (e.g. message buffers)               | `sendStagedTelemetry()`              |
| `parse`      | Breaking down structured input (strings, buffers) into usable fields         | `parseTelemetryPayload()`, `parseKeyValue()` |
| `validate`   | Checking the correctness, range, or format of input or internal data         | `validateChecksum()`, `validateSensorKey()`  |

> 🎯 **Rule of thumb:** If you can't explain a function's role from its name alone, rename it.

---

## 🧭 Prefix Decision Guide

Use this table to help decide which prefix is appropriate for a new function or scheduled task:

| **Action Intent**                          | **Use Prefix** |
|--------------------------------------------|----------------|
| Return current stored value (no work)      | `get`          |
| Read directly from sensor or hardware      | `read`         |
| Refresh internal state or buffers          | `update`       |
| Transmit data externally                   | `send`         |
| Route or act on input                      | `process`      |
| Perform pure math computation              | `calculate`    |
| React to an event or command               | `handle`       |
| Construct a message or composite object    | `build`        |
| Decode structured input                    | `parse`        |
| Check format or range                      | `validate`     |

---

## 🧪 Example Naming in Practice

| **Function Name**                   | **Role**               |
|------------------------------------|------------------------|
| `readLuxSensor()`                  | Gets value via I²C     |
| `updateSensorStaging()`            | Refreshes telemetry buffer |
| `getLatestCoolantTemp()`           | Pure accessor          |
| `calculateAFR()`                   | Computes from inputs   |
| `sendStagedTelemetry()`    | Builds and transmits UDP message |
| `handlePingResponse()`             | Handles event-driven reply |
| `parseTelemetryPayload()`          | Decodes key=value string |
| `validateSensorKey()`              | Confirms known key     |
| `processReceivedTelemetry()`       | Orchestrates full telemetry ingestion |

---

## 🛡️ Best Practices

- Avoid vague verbs like `run`, `execute`, `do`, or `manage`.
- Prefer **`build` + `send`** over `report`, `add`, or `log` for outbound telemetry.
- Never use `read` for functions that don't do actual I/O.
- All scheduled tasks should begin with one of these prefixes.
