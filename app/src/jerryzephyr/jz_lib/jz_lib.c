#include "jz_lib.h"

#include "jz_helpers.h"
#include "ZephyrStringConversion.h"

#include "jerryscript.h"
#include "jerryscript-ext/handlers.h"
#include "jerryscript-ext/properties.h"

#if CONFIG_JZ_FILESYSTEM_HANDLERS
#include "ZephyrFileSystem.h"
#endif
#if CONFIG_JZ_CRYPTO_HANDLERS
#include "ZephyrHash.h"
#endif


int jz_init(void){
    int error = 0;

    jerryx_register_global("print", jerryx_handler_print);

    jz_object_create("console");
    jz_object_add_prop("console", "log", jerryx_handler_print);

    jz_function_create("btoa", btoa_handler);
    jz_function_create("atob", atob_handler);

    #if CONFIG_JZ_FILESYSTEM_HANDLERS
    zephyr_storage_init();
    jz_object_create("fs");
    jz_object_add_prop("fs", "writeFile", writeFile_handler);
    jz_object_add_prop("fs", "readFile", readFile_handler);
    #endif

    #if CONFIG_JZ_CRYPTO_HANDLERS
    jz_object_create("crypto");
    jz_object_add_obj("crypto", "subtle");
    jz_object_prop_add_prop("crypto", "subtle", "digest", digest_handler);
    #endif

    return error;
}