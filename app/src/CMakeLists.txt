
if(CONFIG_JERRYZ_EXAMPLE_THREAD_SIMPLE)
    message(STATUS "Building JerryZ Example Thread (simple)\n")
    target_sources(app PRIVATE js_thread_simple.c)
endif()

if(CONFIG_JERRYZ_EXAMPLE_PRINT_HANDLER)
    message(STATUS "Building JerryZ Example Print Handler\n")
    target_sources(app PRIVATE js_print_handler.c)
endif()

if(CONFIG_JERRYZ_EXAMPLE_SNAPSHOT)
    message(STATUS "Building JerryZ Example Snapshot save and execute\n")
    target_sources(app PRIVATE js_snapshot.c)
endif()
