cmake_minimum_required(VERSION 3.13) # CMAKE (heading)
include(pico_sdk_import.cmake) # CMAKE -> PICO SDK (include)

project(draft C CXX ASM) # CMAKE ("project")
pico_sdk_init() # PICO SDK (initialization)

add_executable(draft
        main.c
        )

pico_generate_pio_header(draft ${CMAKE_CURRENT_LIST_DIR}/i2c.pio)
pico_generate_pio_header(draft ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated) # PICO SDK (PIO)

target_sources(draft PRIVATE
        pio_i2c.c
        pio_i2c.h neopixel.c
        )

# pull in common dependencies

target_link_libraries(draft PRIVATE pico_stdlib hardware_pio)
pico_enable_stdio_usb(draft 1) # PICO SDK (USB)
pico_enable_stdio_uart(draft 0) # PICO SDK (USB)

# create map/bin/hex file etc.
pico_add_extra_outputs(draft)

# add url via pico_set_program_url
example_auto_set_url(draft)
