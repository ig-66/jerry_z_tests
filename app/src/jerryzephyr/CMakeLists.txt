
if(CONFIG_JZ_HELPERS)
    set(JZ_HELPERS_INCLUDE jz_helpers)
    set(JZ_HELPERS_SOURCES jz_helpers/jz_helpers.c)
    zephyr_include_directories(${JZ_HELPERS_INCLUDE})
    zephyr_library_sources(${JZ_HELPERS_SOURCES})
endif()

if(CONFIG_JZ_CRYPTO_HANDLERS)
endif()

if(CONFIG_JZ_FILESYSTEM_HANDLERS)
    set(JZ_FILESYSTEM_HANDLERS_INCLUDE jz_handlers/jz_filesystem_handlers)
    set(JZ_FILESYSTEM_HANDLERS_SOURCES jz_handlers/jz_filesystem_handlers/ZephyrFileSystem.c)
    zephyr_include_directories(${JZ_FILESYSTEM_HANDLERS_INCLUDE})
    zephyr_library_sources(${JZ_FILESYSTEM_HANDLERS_SOURCES}) 
endif()

set(JZ_CONVERSION_HANDLERS_INCLUDE jz_handlers/jz_conversion_handlers)
set(JZ_CONVERSION_HANDLERS_SOURCES jz_handlers/jz_conversion_handlers/ZephyrStringConversion.c)
zephyr_include_directories(${JZ_CONVERSION_HANDLERS_INCLUDE})
zephyr_library_sources(${JZ_CONVERSION_HANDLERS_SOURCES}) 