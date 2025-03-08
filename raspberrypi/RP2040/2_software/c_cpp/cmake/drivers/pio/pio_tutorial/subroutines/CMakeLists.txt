add_executable(subroutine)

pico_generate_pio_header(subroutine ${CMAKE_CURRENT_LIST_DIR}/subroutine.pio)

target_sources(subroutine PRIVATE subroutine.cpp)

target_link_libraries(subroutine PRIVATE
        pico_stdlib
        hardware_pio
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(subroutine 1)
pico_enable_stdio_uart(subroutine 0)

pico_add_extra_outputs(subroutine)
