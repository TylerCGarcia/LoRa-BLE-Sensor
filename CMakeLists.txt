# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)


# Add local board directory to selection
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(BOARD lora_ble_sensor)

# Set the overlay file
set(DTC_OVERLAY_FILE 
    ${CMAKE_CURRENT_SOURCE_DIR}/conf/pmic.overlay)

# Get the version from Git
find_package(Git QUIET)
if(GIT_FOUND)
  execute_process(
    COMMAND git describe --tags --long
    WORKING_DIRECTORY                ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE                  version
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE
    ERROR_VARIABLE                   stderr
    RESULT_VARIABLE                  return_code
  )
  if(return_code)
    message(STATUS "git describe failed: ${stderr}; ${KERNEL_VERSION_STRING} will be used instead")
  elseif(CMAKE_VERBOSE_MAKEFILE)
    message(STATUS "git describe stderr: ${stderr}")
  endif()
endif()

if(version)
    string(REGEX REPLACE "^([0-9]+).*$" "\\1" version_major ${version})
    string(REGEX REPLACE "^[0-9]+\.([0-9]+).*$" "\\1" version_minor "${version}")
    string(REGEX REPLACE "^[0-9]+\.[0-9]+\.([0-9]+).*$" "\\1" version_patch "${version}")
    string(REGEX REPLACE "^[0-9]+\.[0-9]+\.[0-9]+-([0-9]+)-.*$" "\\1" version_commit "${version}")
    string(REGEX REPLACE "^[0-9]+\.[0-9]+\.[0-9]+-[0-9]+-(.*)$" "\\1" version_hash "${version}")

    # Create version.conf
    file(WRITE conf/version.conf "CONFIG_APP_VERSION_MAJOR=${version_major}\n")
    file(APPEND conf/version.conf "CONFIG_APP_VERSION_MINOR=${version_minor}\n")
    file(APPEND conf/version.conf "CONFIG_APP_VERSION_PATCH=${version_patch}\n")
    file(APPEND conf/version.conf "CONFIG_APP_VERSION_COMMIT=${version_commit}\n")
    file(APPEND conf/version.conf "CONFIG_APP_VERSION_HASH=\"${version_hash}\"\n")
    file(APPEND conf/version.conf "CONFIG_APP_VERSION=\"${version}\"\n")

    message(STATUS "Version: ${version_major}.${version_minor}.${version_patch}-${version_commit}-${version_hash}")
endif()

if(BOARD STREQUAL "lora_ble_sensor")
    message(STATUS "Building for LoRa-BLE-Sensor")
else()
    message(STATUS "Building for ${BOARD}")
endif()

# Define configuration files.
list(APPEND CONF_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/conf/version.conf
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sensor_app.c
)
