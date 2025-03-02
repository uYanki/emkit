add_executable(sm_to_dma_to_sm_to_dma_to_buffer)

pico_generate_pio_header(sm_to_dma_to_sm_to_dma_to_buffer ${CMAKE_CURRENT_LIST_DIR}/sm_to_dma_to_sm_to_dma_to_buffer.pio)

target_sources(sm_to_dma_to_sm_to_dma_to_buffer PRIVATE sm_to_dma_to_sm_to_dma_to_buffer.cpp)

target_link_libraries(sm_to_dma_to_sm_to_dma_to_buffer PRIVATE
        pico_stdlib
        hardware_pio
        hardware_dma
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(sm_to_dma_to_sm_to_dma_to_buffer 1)
pico_enable_stdio_uart(sm_to_dma_to_sm_to_dma_to_buffer 0)

pico_add_extra_outputs(sm_to_dma_to_sm_to_dma_to_buffer)

# add url via pico_set_program_url
example_auto_set_url(sm_to_dma_to_sm_to_dma_to_buffer)


