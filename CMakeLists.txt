# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)


# Add local board directory to selection
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(BOARD lora_ble_sensor)

# Set the overlay file
set(DTC_OVERLAY_FILE 
    ${CMAKE_CURRENT_SOURCE_DIR}/conf/pmic.overlay)

# Define configuration files.
list(APPEND CONF_FILE
  ${CMAKE_CURRENT_SOURCE_DIR}/conf/prj.conf)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(LoRa-BLE-Sensor)

target_include_directories(app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensor_power.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensor_reading.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensor_lorawan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensor_ble.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ble_sensor_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ble_lorawan_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensor_nvs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensor_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensor_scheduling.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensor_data.c
)
