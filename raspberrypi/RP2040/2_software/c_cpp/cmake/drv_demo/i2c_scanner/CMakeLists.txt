cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
add_link_options("-Wl,--print-memory-usage")

project(i2c_scanner C CXX ASM)

pico_sdk_init()

add_executable(${PROJECT_NAME} 
        main.c
)
# pull in common dependencies and additional i2c hardware support
target_link_libraries(${PROJECT_NAME} PUBLIC pico_stdlib hardware_i2c)

pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})




