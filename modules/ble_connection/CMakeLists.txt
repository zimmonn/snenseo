
if(CONFIG_SNENSEO_BLE_CONNECTION)
    # Add your include directory
    zephyr_include_directories(.)

    # Add the source file you want to compile
        zephyr_library_sources(
        snenseo_ble_connection.c
        snenseo_ble_svc.c
    )
endif()
