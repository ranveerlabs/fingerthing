pico_enable_stdio_uart(pico_fido2 0)
if(FINGERTHING_SENSOR)
    target_sources(pico_fido2 PRIVATE
        "${FINGERTHING_DIR}/src/r503.c"
        "${FINGERTHING_DIR}/src/finger.c"
        "${FINGERTHING_DIR}/src/port.c")
    target_include_directories(pico_fido2 PRIVATE "${FINGERTHING_DIR}/src")
    target_compile_definitions(pico_fido2 PRIVATE FINGERTHING_R503)
    target_link_libraries(pico_fido2 PRIVATE hardware_uart)
endif()
