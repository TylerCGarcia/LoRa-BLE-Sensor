# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(sensor_app_tests)

target_include_directories(app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/fakes
)

target_sources(app PRIVATE src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/sensor_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/sensor_scheduling.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/sensor_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/sensor_nvs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/sensor_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fakes/sensor_ble_fakes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fakes/sensor_power_fakes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fakes/sensor_reading_fakes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fakes/sensor_lorawan_fakes.c
)
