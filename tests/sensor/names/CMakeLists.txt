# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(sensor_power_tests)

target_include_directories(app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../app/include
)

target_sources(app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/sensor_names.c
)
