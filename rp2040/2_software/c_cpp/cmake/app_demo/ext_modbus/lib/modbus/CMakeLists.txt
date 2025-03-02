add_library(modbus
        src/modbus.c
        src/mbslv.c
        src/mbmst.c
        )

target_include_directories(modbus PUBLIC inc)
