# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(sensor_pmic_tests)

target_include_directories(app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../app/include
    ${CMAKE_CURRENT_SOURCE_DIR}/fakes
)

target_sources(app PRIVATE src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../app/src/sensor_pmic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fakes/sensor_led_fakes.c
)