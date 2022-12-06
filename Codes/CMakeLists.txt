
project(project C CXX ASM) # CMAKE ("project")
pico_sdk_init() # PICO SDK (initialization)

add_executable(project
        project.c
        )

target_sources(project PRIVATE project.c sound.c servo.c) # CMAKE -> C (compiler)
target_link_libraries(project PRIVATE pico_stdlib hardware_pwm pico_stdio hardware_adc)
pico_enable_stdio_usb(project 1) # PICO SDK (USB)
pico_enable_stdio_uart(project 0) # PICO SDK (USB)

# create map/bin/hex file etc.
pico_add_extra_outputs(project)

# add url via pico_set_program_url
example_auto_set_url(project)
