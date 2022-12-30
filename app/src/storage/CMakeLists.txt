target_sources(app PRIVATE storage.c)

if(CONFIG_FIELDFARE)
    target_sources(app PRIVATE ZephyrFileSystem.c)
endif()