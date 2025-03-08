include_directories(${USDK_INCLUDE_DIR})

add_library(i2cdev
    src/i2c.c
    src/swi2c.c
    src/hwi2c.c
)

target_include_directories(i2cdev PUBLIC inc)

target_link_libraries(i2cdev 
    pico_stdlib
    hardware_gpio
    hardware_i2c
)
