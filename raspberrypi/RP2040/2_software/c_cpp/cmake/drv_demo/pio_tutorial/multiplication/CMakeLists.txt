add_executable(multiplier)

pico_generate_pio_header(multiplier ${CMAKE_CURRENT_LIST_DIR}/multiplier.pio)

target_sources(multiplier PRIVATE multiplier.cpp)

target_link_libraries(multiplier PRIVATE
        pico_stdlib
        hardware_pio
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(multiplier 1)
pico_enable_stdio_uart(multiplier 0)

pico_add_extra_outputs(multiplier)

# add url via pico_set_program_url
example_auto_set_url(multiplier)


