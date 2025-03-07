add_library(tm1638
    src/tm1638drv.cpp
    src/tm1638.cpp
)

target_include_directories(tm1638 PUBLIC inc)

target_link_libraries(tm1638 PRIVATE
    pico_stdlib
    hardware_gpio
)
