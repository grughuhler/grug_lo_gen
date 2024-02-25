add_executable(grug_lo_gen_pio)

# enable usb output, disable uart output
pico_enable_stdio_usb(grug_lo_gen_pio 1)
pico_enable_stdio_uart(grug_lo_gen_pio 0)


pico_generate_pio_header(grug_lo_gen_pio ${CMAKE_CURRENT_LIST_DIR}/grug_lo_gen.pio)

target_sources(grug_lo_gen_pio PRIVATE grug_lo_gen.c)

target_link_libraries(grug_lo_gen_pio PRIVATE
        pico_stdlib
        hardware_pio)

pico_add_extra_outputs(grug_lo_gen_pio)

# add url via pico_set_program_url
example_auto_set_url(grug_lo_gen_pio)
