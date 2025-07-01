# LoRa-BLE-Sensor
Hardware: LoRa-BLE-Sensor

# Overview
An nRF52 and SX1262 based LoRaWAN node that uses 2 configurable sensors, each with its own configurable power supply. The firmware for this node is built around the Zephyr RTOS. 
ZTEST and TWISTER are used for creating and running the unit tests for the device. This project was built on linux and some components, such as using native_sim for the unit tests requires linux.

# Project Initialization

As a prerequisite, the [zephyr sdk](https://docs.zephyrproject.org/latest/develop/toolchains/zephyr_sdk.html) needs to be installed.

First clone this repository:

```bash
git clone https://github.com/TylerCGarcia/LoRa-BLE-Sensor.git
```

After cloning the repository, run the following command in the projects root directory:

```bash
scripts/init_project.sh
```

This will initialize the python environment(.venv), initialize west(.west), and update west, pulling down the appropriate zephyr dependencies needed for this project. These dependencies are determined by the west manifest(west.yml) in the ``zephyr`` folder. The update process may take time, when finished all of the dependencies should be located in the ``deps`` folder. After this the ``app`` folder can be built and twister can be used to run the ``tests`` folder.

# Code Breakdown
**main.c** 
- Calls functions in **sensor_app** library.
  * Initializes the bluetooth thread.
  * Switches between states for the sensor(configuration, running, error).

**sensor_app.c** 
- Calls functions in **sensor_nvs**, **sensor_scheduling**, **sensor_lorawan** and **sensor_data**.
- Controles the high level functionality of the project.

**sensor_ble.c** 
- Handles ble setup and configurations for both advertising and connections.

**ble_device_service.c**
- BLE service with configurations for the device service.
  * This handles the device name, hw model, fw version and battery information.

**ble_lorawan_service.c**
- BLE service for configuring lorawan.
  * This configures dev_eui, join_eui, app_key, and data frequency.

**ble_sensor_service.c**
- BLE service for configuring and reading the sensors.
  * This configures sensor type, sensor voltage, sensor read frequency.
  * The sensor data and its timestamp can be read in this characteristic as well.

**sensor_data.c** 
- Calls functions in **sensor_reading** and **sensor_power**.
- Manages power control in relation to sensor readings.
  * Manages the data buffers from sensor readings

**sensor_reading.c** 
- Low level functionality for reading from sensors.
- Can be used with a variable number of sensors.

**sensor_power.c** 
- Low level functionality for controlling power outputs to sensors.
- Manages regulators onboard regulators.

**sensor_scheduling.c** 
- Calls functions in **sensor_timer**.
- Manages scehduling of sensor readings and lorawan transmissions.

**sensor_timer.c** 
- Low level functionality to handle timers and their alarms.
- Is intended for use with one timer that has multiple alarms.

**sensor_pmic.c** 
- Handles the pmic's charge controller, battery sensors, and LED controller.

**sensor_nvs.c** 
- Handles the sensor configurations that are stored in non-volatile memory.

**sensor_names.c** 
- Converts sensor identifiers(type and voltage) from their corresponding codes to written language.
  * Mainly used for easier debugging of bluetooth and easier calls for viewing logs.

# Power Analysis
- No power analysis is done yet

# Bugs/To-Do
- Add nPM1300 functionality
  * Current, and Charge of the battery
  * Enable Charging
- Add NFC for BLE Configurations
- Error handling when state given is out of bounds

# Revision History
Current Version: **V0.4.1**

Version Breakdown: `[Major API Change]`.`[Minor Change/Added Functionality]`.`[Patch/bug fixes]`

### V0.4.1
- Switched SDK to nRF Connect SDK V3.0.2
  * Some functionality needed to change(e.g. resetting counter in **sensor_timer.h**)
- Cleaned code to remove compile warnings

### V0.4.0
- PMIC LED is used for running an configuration state
- PMIC battery voltage, temperature, current usb connections are added
  * Battery voltage and battery temperature are sent over ble and lora

### V0.3.0
- BLE DFU is implemented using SMP Server
  * MCUMGR is what controls this
- Logging for MCUBOOT is used with USB
  * lora_ble_sensor flash partitions are edited to give mcuboot enough memory for usb logging

### V0.2.2
- Names for sensor types and output voltages added to their own library
  * Relates indexes to strings to make the ble interaction more human readable
- MCUBOOT is added to project
  * For later use with OTA updates

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
