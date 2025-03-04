cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

##########################################################

project(blink C CXX ASM)

add_executable(blink
        blink.c
        )

# pull in common dependencies
target_link_libraries(blink pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(blink)

pico_set_linker_script(blink ${CMAKE_CURRENT_SOURCE_DIR}/noboot2.ld)

##########################################################

project(hello_serial C CXX ASM)

add_executable(hello_serial
        hello_serial.c
        )

# pull in common dependencies
target_link_libraries(hello_serial pico_stdlib)

# disable usb output, enable uart output
pico_enable_stdio_usb(hello_serial 0)
pico_enable_stdio_uart(hello_serial 1)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(hello_serial)

pico_set_linker_script(hello_serial ${CMAKE_CURRENT_SOURCE_DIR}/noboot2.ld)

##########################################################

project(hello_usb C CXX ASM)

add_executable(hello_usb
            hello_usb.c
            )

# pull in common dependencies
target_link_libraries(hello_usb pico_stdlib)

# enable usb output, disable uart output
pico_enable_stdio_usb(hello_usb 1)
pico_enable_stdio_uart(hello_usb 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(hello_usb)

pico_set_linker_script(hello_usb ${CMAKE_CURRENT_SOURCE_DIR}/noboot2.ld)
