#include "include/js_snapshot.h"

#include <zephyr.h>
#include "jerryscript.h"
#include "jerryscript-ext/handlers.h"
#include "jerryscript-ext/properties.h"


int main(){

    js_example_snapshot();

    return 0;
}


int js_example_snapshot(void){
    
    printk("*** JerryScript Snapshot Test ***\n\n");

    /* Initialize Jerry engine */
    jerry_init(JERRY_INIT_EMPTY);

    /* Register the callback for 'print' */
    jerryx_register_global ("print", jerryx_handler_print);
    
    /* Script to be used for the snapshot */
    const jerry_char_t script[] = "print('This is a Snapshot!');";

    uint32_t snap_buffer[256];

    /* Parse de script */
    jerry_value_t parsed_code = jerry_parse(script, sizeof(script)-1, NULL);
    if (jerry_value_is_exception(parsed_code)){
        printk("*** Parsed code is an exception\n");
        return -1;
    }
    printk("*** Parse: OK!\n");
    
    /* Generate snapshot from the parsed script */
    jerry_value_t gen_res = jerry_generate_snapshot(parsed_code, 0, snap_buffer, sizeof(snap_buffer)-1);
    jerry_value_free(parsed_code);


    if(!(!jerry_value_is_exception(gen_res) && jerry_value_is_number(gen_res))){
        printk("*** Generate Snapshot: ERROR: Exception XXX\n");
        printk("        %s", jerry_value_is_number(gen_res) ? "Result NaN ": "");
        printk("        %s", jerry_value_is_exception(gen_res) ? "Result is Exception": "");
        printk("\n");
        return -1;
    }
    printk("*** Generate Snapshot: OK!\n");

    size_t snapshot_size = (size_t) jerry_value_as_number (gen_res);
    jerry_value_free (gen_res);

    jerry_value_t result = jerry_exec_snapshot (snap_buffer,
                                                snapshot_size,
                                                0,
                                                JERRY_SNAPSHOT_EXEC_ALLOW_STATIC,
                                                0);

    if(!jerry_value_is_exception(result)){
        printk("*** Execute Snapshot: OK!\n");
    } else {
        printk("*** Execute Snapshot: ERROR: Exception XXX\n");
    }

    jerry_value_free(result);
    
    /* Kill jerry engine */
    jerry_cleanup();

    printk("*** End of JerryScript Snapshot Test ***\n");
    return 0;
}