cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

include(example_auto_set_url.cmake)

add_subdirectory(sm_to_dma_to_buffer)
add_subdirectory(sm_to_dma_to_sm_to_dma_to_buffer)
add_subdirectory(subroutines)
add_subdirectory(two_pio_programs_one_file)
add_subdirectory(two_sm_one_disabled)
add_subdirectory(two_sm_one_disabled_with_irq)
add_subdirectory(two_sm_simple)
add_subdirectory(multiplication)

