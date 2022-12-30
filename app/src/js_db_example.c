#include "storage/js_kvdb.h"
#include "storage/ZephyrFileSystem.h"

#include <zephyr.h>

// JerryScript Library
#include "jerryscript.h"
#include "jerryscript-ext/handlers.h"
#include "jerryscript-ext/properties.h"

int main(void){

	const jerry_char_t script[] = "class File { \
			init() { \
				fs_init(); \
			} \
			writeFile(key, value, func) { \
				fs_write(key, value, func); \
			}\
			readFile(key, func) { \
				fs_read(key, func); \
			}\
		} \
		const file = { \
			init() { \
				fs_init(); \
			}, \
			writeFile(key, value, func) { \
				fs_write(key, value, func); \
			} \
		}; \
		fs = new File; \
		fs.init(); \
		var data = 'value'; \
		new Promise((resolve, reject) => { \
            fs.writeFile('/SD:/key9.nvd', data, function (err) { \
                if(err) { \
					print('promise rejected'); \
                    reject(err); \
                } else { \
					print('promise resolved'); \
                    resolve(); \
                } \
            }); \
        }); \
		new Promise((resolve, reject) => { \
            fs.readFile('/SD:/key9.nvd', function (resolve, err) { \
                if(resolve) { \
					print('promise resolved'); \
                    resolve(); \
                } else { \
					print('promise rejected'); \
                    reject(err); \
                } \
            }); \
        }); \
		print('end'); \
	";

	const jerry_length_t script_size = sizeof(script) - 1;

	/* Initialize engine */
	jerry_init(JERRY_INIT_EMPTY);

	jerryx_register_global("print", jerryx_handler_print);

	jerryx_register_global("fs_init", fs_init_handler);
	jerryx_register_global("fs_write", fileWrite_handler);
	jerryx_register_global("fs_read", fileRead_handler);

	/* Setup Global scope code */
	jerry_value_t parsed_code = jerry_parse(script, script_size, NULL);

	if (!jerry_value_is_exception(parsed_code))
	{
		/* Execute the parsed source code in the Global scope */
		printk("Parsed code is not an exception!\n");
		jerry_value_t ret_value = jerry_run(parsed_code);

		/* Returned value must be freed */
		jerry_value_free(ret_value);
	} else {
		printk("--- ERROR: Parsed code is an exception!\n");
	}

	/* Parsed source code must be freed */
	jerry_value_free(parsed_code);

	/* Cleanup engine */
	jerry_cleanup();

	return 0;
}