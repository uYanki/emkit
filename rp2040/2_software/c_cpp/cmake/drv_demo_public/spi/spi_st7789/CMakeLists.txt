add_executable(spi_st7789
        spi_st7789.c
        )

# pull in common dependencies and additional other hardware support
target_link_libraries(spi_st7789 pico_stdlib hardware_spi hardware_dma hardware_pwm)

# create map/bin/hex file etc.
pico_add_extra_outputs(spi_st7789)

# add url via pico_set_program_url
example_auto_set_url(spi_st7789)
