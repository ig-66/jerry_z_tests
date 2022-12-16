#include "include/js_print_handler.h"

#include <zephyr.h>

// JerryScript Library
#include "jerryscript.h"
#include "jerryscript-ext/handlers.h"
#include "jerryscript-ext/properties.h"

int main(){

	js_example_print_handler();

	return;
}


int js_example_print_handler (void) 
{
	
	const jerry_char_t script[] = "var a = 5; \
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
