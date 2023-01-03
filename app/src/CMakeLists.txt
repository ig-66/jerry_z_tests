
if(CONFIG_JERRYZ_EXAMPLE_THREAD_SIMPLE)
    message(STATUS "Building JerryZ Example Thread (simple)\n")
    target_sources(app PRIVATE js_thread_simple.c)
endif()

if(CONFIG_JERRYZ_EXAMPLE_PRINT_HANDLER)
    message(STATUS "Building JerryZ Example Print Handler\n")
    target_sources(app PRIVATE js_print_handler.c)
endif()

if(CONFIG_JERRYZ_EXAMPLE_SNAP_LOAD)
    if((NOT CONFIG_DISK_DRIVER_FLASH) AND (NOT CONFIG_DISK_DRIVER_SDMMC))
        message( FATAL_ERROR "No disk access settings detected." )
    endif()
    
    message(STATUS "Building JerryZ Example Snapshot file load\n")
    target_sources(app PRIVATE js_snapshot_load.c)
    target_sources(app PRIVATE storage/storage.c)
endif()

if(CONFIG_JERRYZ_EXAMPLE_SNAPSHOT)
    message(STATUS "Building JerryZ Example Snapshot save and execute\n")
    target_sources(app PRIVATE js_snapshot.c)
endif()

if(CONFIG_JERRYZ_SETTIMEOUT_TEST)
    message(STATUS "+---------------------------------+")
    message(STATUS "|  Building the setTimeout test!  |")
    message(STATUS "+---------------------------------+")
    target_sources(app PRIVATE js_settimeout_test.c)
endif()

if(CONFIG_FIELDFARE)
    message(STATUS "+---------------------------------+")
    message(STATUS "|   Building the fieldfare test!  |")
    message(STATUS "+---------------------------------+")
    target_sources(app PRIVATE js_db_example.c)
    
    set(JERRY_ZEPHYR_FS_INCLUDE storage)
    set(JERRY_ZEPHYR_FS_SOURCE storage/ZephyrFileSystem.c)
    zephyr_include_directories(${JERRY_ZEPHYR_FS_INCLUDE})
    zephyr_library_sources(${JERRY_ZEPHYR_FS_SOURCE})
endif()
