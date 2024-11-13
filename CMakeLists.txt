# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_BUILD_TYPE "Release") 

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(PICO_CXX_ENABLE_EXCEPTIONS 0)
set(PICO_CXX_ENABLE_RTTI 0)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(pico-FDCT C CXX ASM)

pico_sdk_init()

add_executable(pico-fdct-test)

# set(CMAKE_CXX_FLAGS "-Ofast")

target_sources(pico-fdct-test PUBLIC
               ${CMAKE_CURRENT_LIST_DIR}/src/debug/StampPrintf.cpp
               ${CMAKE_CURRENT_LIST_DIR}/src/hw/init.cpp
               ${CMAKE_CURRENT_LIST_DIR}/src/test.cpp
              )

pico_set_program_name(pico-fdct-test "pico-fdct")
pico_set_program_version(pico-fdct-test "0.1")

pico_enable_stdio_uart(pico-fdct-test 1)
pico_enable_stdio_usb(pico-fdct-test 1)

# Add the standard include files to the build
target_include_directories(pico-fdct-test PUBLIC
                           ${CMAKE_CURRENT_LIST_DIR}
                           ${CMAKE_CURRENT_LIST_DIR}/src/debug
                           ${CMAKE_CURRENT_LIST_DIR}/src/macros
                           ${CMAKE_CURRENT_LIST_DIR}/src/hw
                           ${CMAKE_CURRENT_LIST_DIR}/src/sigproc
                           ${CMAKE_CURRENT_LIST_DIR}/src/util
                           ${CMAKE_CURRENT_LIST_DIR}/..
)

target_link_libraries(pico-fdct-test
                      pico_stdlib
                      pico_sync
                      hardware_timer
                      hardware_clocks
                      pico_divider
                     )

target_compile_options(pico-fdct-test PUBLIC -fno-exceptions)

pico_add_extra_outputs(pico-fdct-test)
