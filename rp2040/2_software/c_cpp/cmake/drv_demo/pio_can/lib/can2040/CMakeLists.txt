add_library(can2040
    src/can2040.c
)

target_include_directories(can2040 PUBLIC src)

pico_generate_pio_header(can2040  
    ${CMAKE_CURRENT_LIST_DIR}/src/can2040.pio
)

target_link_libraries(can2040 PRIVATE
    cmsis_core 
    hardware_gpio
    hardware_pio 
    hardware_dma
)
