# Import SDK
cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(bootsel-reboot)
pico_sdk_init()

# Build application
add_executable(
	${PROJECT_NAME}
	main.cpp
	bootsel-reboot.cpp
)

# Debug output - Enable for USB CDC and disable for on-board UART
# You can change these around (or even have both active) by changing the 1s and 0s
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# Build dependencies and link application
pico_add_extra_outputs(${PROJECT_NAME})
target_link_libraries(
	${PROJECT_NAME}
	pico_stdlib
)