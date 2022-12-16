
#include <zephyr/zephyr.h>

#include "jerryscript.h"
#include "jerryscript-ext/handlers.h"
#include "jerryscript-ext/properties.h"

/* 10000 msec = 10 sec */
#define SLEEP_TIME_MS   10000

#define JS_STACK_SIZE 4096
#define JS_PRIORITY 5

extern void jerry_start(void *, void *, void *);


/* Start Jerry engine on a new thread */
K_THREAD_DEFINE(js_tid, JS_STACK_SIZE,
                jerry_start, NULL, NULL, NULL,
                JS_PRIORITY, 0, 0);


int js_example_print_handler(void);


void main(void){

	while (1) {
		k_msleep(SLEEP_TIME_MS);
        printk("PING from main()\n");
	}
}


void jerry_start(void * unused1, void * unused2, void * unused3){

    // /* Init jerry engine */
    // jerry_init(JERRY_INIT_EMPTY);

    // /* Register print function */
    // jerryx_register_global ("print", jerryx_handler_print);

    while(1){
        // check for timers and execute them!
        k_msleep(1000);

	    js_example_print_handler();

    }
}


int js_example_print_handler (void) 
{
	
	const jerry_char_t script[] = "print('This is from the Jerry Thread!'); \
        var a = 5; \
		print('variable = ' + a); \
		var b = 10; \
		print('This is double = ' + b); \
		print('Oh shit! It works!!!!!!!!'); \
	";

	const jerry_length_t script_size = sizeof (script) - 1;

	/* Initialize engine */
	jerry_init (JERRY_INIT_EMPTY);

	jerryx_register_global ("print", jerryx_handler_print);

	/* Setup Global scope code */
	printk("before parse\n");
	jerry_value_t parsed_code = jerry_parse (script, script_size, NULL);
	printk("after parse\n");

	if (!jerry_value_is_exception (parsed_code))
	{
		/* Execute the parsed source code in the Global scope */
		printk("Parsed code is not an exception!\n");
		jerry_value_t ret_value = jerry_run (parsed_code);

		/* Returned value must be freed */
		jerry_value_free (ret_value);
	} else {
		printk("--- ERROR: Parsed code is an exception!\n");
	}

	/* Parsed source code must be freed */
	jerry_value_free (parsed_code);

	/* Cleanup engine */
	jerry_cleanup ();

	return 0;
}
