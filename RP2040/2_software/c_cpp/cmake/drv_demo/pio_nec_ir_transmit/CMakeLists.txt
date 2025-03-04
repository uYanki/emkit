cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
add_link_options("-Wl,--print-memory-usage")

project(pio_ir_transmit C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_subdirectory(nec_transmit)

add_executable (${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME}
  pico_stdlib
  hardware_pio
  nec_transmit
)

pico_add_extra_outputs(${PROJECT_NAME})

