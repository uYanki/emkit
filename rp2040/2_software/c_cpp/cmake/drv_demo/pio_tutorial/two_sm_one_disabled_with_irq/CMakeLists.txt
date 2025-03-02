add_executable(two_sm_one_disabled_with_irq)

pico_generate_pio_header(two_sm_one_disabled_with_irq ${CMAKE_CURRENT_LIST_DIR}/two_sm_one_disabled_with_irq.pio)

target_sources(two_sm_one_disabled_with_irq PRIVATE two_sm_one_disabled_with_irq.cpp)

target_link_libraries(two_sm_one_disabled_with_irq PRIVATE
        pico_stdlib
        hardware_pio
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(two_sm_one_disabled_with_irq 1)
pico_enable_stdio_uart(two_sm_one_disabled_with_irq 0)

pico_add_extra_outputs(two_sm_one_disabled_with_irq)

# add url via pico_set_program_url
example_auto_set_url(two_sm_one_disabled_with_irq)


