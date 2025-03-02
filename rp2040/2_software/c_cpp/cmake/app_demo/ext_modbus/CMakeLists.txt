cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
add_link_options("-Wl,--print-memory-usage")

project(mbpico C CXX ASM)

pico_sdk_init()

add_executable(${PROJECT_NAME} 
        src/main.c
        src/para_group.c
        src/para_table.c
        src/para_table.c
        src/usb_desc.c
)
include_directories(inc)
# target_include_directories(${PROJECT_NAME} PUBLIC inc)

add_subdirectory(lib)
target_link_libraries(${PROJECT_NAME} PUBLIC modbus pid pico_stdlib tinyusb_device tinyusb_board)

pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

pico_add_extra_outputs(${PROJECT_NAME})