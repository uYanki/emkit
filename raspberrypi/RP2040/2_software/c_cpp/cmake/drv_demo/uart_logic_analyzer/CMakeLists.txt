# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Pull in Pico SDK (must be before project)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
add_link_options("-Wl,--print-memory-usage")

project(rp2040_logic_analyzer  C CXX)

# Initialise the Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(${PROJECT_NAME} main.c)

pico_enable_stdio_uart(${PROJECT_NAME}  0)
pico_enable_stdio_usb(${PROJECT_NAME}  1)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME}  pico_stdlib hardware_pio hardware_dma)

pico_add_extra_outputs(${PROJECT_NAME} )

