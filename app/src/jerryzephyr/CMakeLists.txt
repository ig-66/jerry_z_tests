
if(CONFIG_JZ_CRYPTO_HANDLERS)
    set(JZ_CRYPTO_HANDLERS_INCLUDE jz_handlers/jz_crypto_handlers)
    set(JZ_CRIPTO_HANDLERS_SOURCES jz_handlers/jz_crypto_handlers/ZephyrHash.c)
    zephyr_include_directories(${JZ_CRYPTO_HANDLERS_INCLUDE})
    zephyr_library_sources(${JZ_CRIPTO_HANDLERS_SOURCES})
endif()

if(CONFIG_JZ_FILESYSTEM_HANDLERS)
    set(JZ_FILESYSTEM_HANDLERS_INCLUDE jz_handlers/jz_filesystem_handlers)
    set(JZ_FILESYSTEM_HANDLERS_SOURCES jz_handlers/jz_filesystem_handlers/ZephyrFileSystem.c)
    zephyr_include_directories(${JZ_FILESYSTEM_HANDLERS_INCLUDE})
    zephyr_library_sources(${JZ_FILESYSTEM_HANDLERS_SOURCES}) 
endif()

set(JZ_HELPERS_INCLUDE jz_helpers)
set(JZ_HELPERS_SOURCES jz_helpers/jz_helpers.c)
zephyr_include_directories(${JZ_HELPERS_INCLUDE})
zephyr_library_sources(${JZ_HELPERS_SOURCES})

set(JZ_CONVERSION_HANDLERS_INCLUDE jz_handlers/jz_conversion_handlers)
set(JZ_CONVERSION_HANDLERS_SOURCES jz_handlers/jz_conversion_handlers/ZephyrStringConversion.c)
zephyr_include_directories(${JZ_CONVERSION_HANDLERS_INCLUDE})
zephyr_library_sources(${JZ_CONVERSION_HANDLERS_SOURCES}) 

zephyr_include_directories(jz_lib)
zephyr_library_sources(jz_lib/jz_lib.c) 