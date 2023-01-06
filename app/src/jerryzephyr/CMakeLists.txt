
if(CONFIG_JZ_HELPERS)
endif()

if(CONFIG_JZ_CRYPTO_HANDLERS)
endif()

if(CONFIG_JZ_FILESYSTEM_HANDLERS)
    set(JZ_FILESYSTEM_HANDLERS_INCLUDE jz_handlers/jz_filesystem_handlers)
    set(JZ_FILESYSTEM_HANDLERS_SOURCES jz_handlers/jz_filesystem_handlers/ZephyrFileSystem.c)
    zephyr_include_directories(${JZ_FILESYSTEM_HANDLERS_INCLUDE})
    zephyr_library_sources(${JZ_FILESYSTEM_HANDLERS_SOURCES}) 
endif()
