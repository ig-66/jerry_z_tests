#ifndef _ZEPHYRFILESYSTEM_H_
#define _ZEPHYRFILESYSTEM_H_

#include "jerryscript.h"

/*  Create the fs (file System) object
*   
*   @return 0 if successful, otherwise failed
*/
int jz_fs_obj_create(void);

jerry_value_t
fs_init_handler(const jerry_call_info_t *call_info_p,
				const jerry_value_t arguments[],
				const jerry_length_t arguments_count);

jerry_value_t
writeFile_handler(const jerry_call_info_t *call_info_p,
			const jerry_value_t arguments[],
			const jerry_length_t arguments_count);

jerry_value_t
readFile_handler(const jerry_call_info_t *call_info_p,
			const jerry_value_t arguments[],
			const jerry_length_t arguments_count);

#endif //_ZEPHYRFILESYSTEM_H_