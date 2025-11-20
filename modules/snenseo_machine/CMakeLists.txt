
if(CONFIG_SNENSEO_MACHINE)
    # Add your include directory
    zephyr_include_directories(.)

    # Add the source file you want to compile
    zephyr_library_sources(
        snenseo_machine.c
    )
endif()
