# LoRa-BLE-Sensor
Hardware: LoRa-BLE-Sensor

# Overview
An nRF52 and SX1262 based LoRaWAN node that uses 2 configurable sensors, each with its own configurable power supply. The firmware for this node is built around the Zephyr RTOS. 
ZTEST and TWISTER are used for creating and running the unit tests for the device.

# Code Breakdown
**main.c** 
- Calls functions in **sensor_app** library
  * Switches between states for the sensor

# Power Analysis
- No power analysis is done yet

# Bugs/To-Do
- Add OTA Functionality
- Add nPM1300 functionality
  * Voltage, Current, Temperature, and Charge of the battery
  * Controlling LED

# Revision History
Current Version: **V0.2.1**

Version Breakdown: `[Major API Change]`.`[Minor Change/Added Functionality]`.`[Patch/bug fixes]`

### V0.2.1
- Changes made for low power consumption
  * Digital inputs changed to active low in dts 
  * CONFIF_PM_DEVICE enabled

### V0.2.0
- version.conf file is created from git tags when device is built
  * the code for this is in CMakeLists.txt
- Firmware and hardware versions added to LoRaWAN packet
- Sensor Configurations added to LoRaWAN packet

### V0.1.0
- Initial working version of the code
  * All lorawan setup and send functionality is in place
  * Both sensors can be fully configured to VOLTAGE_SENSOR, CURRENT_SENSOR, or PULSE_SENSOR
  * Both sensors can be switched from no power, 3.3V, 5V, 6V, 12V or 24V
  * BLE functionality works for configuring the LoRaWAN and Sensors
  * Scheduling and timing system is fully working
