cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(usb_uart_bridge)

pico_sdk_init()
  
add_executable(${PROJECT_NAME} uart-bridge.c usb-descriptors.c)

target_include_directories(${PROJECT_NAME} PUBLIC
	./
	$ENV{PICO_SDK_PATH}/lib/tinyusb/src)

target_link_libraries(${PROJECT_NAME}
	pico_multicore
	pico_stdlib
	tinyusb_device)

pico_add_extra_outputs(${PROJECT_NAME})
