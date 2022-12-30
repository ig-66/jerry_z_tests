#ifndef _ZEPHYRFILESYSTEM_H_
#define _ZEPHYRFILESYSTEM_H_

#include "jerryscript.h"

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

/* Initialize the Zephyr storage
*/
int zephyr_storage_init(void);

/*  Create and/or write a file with data
 *
 *   Returns > 0 - on success
 *   Returns < 0 - on error (error code)
 */
int zephyr_storage_write_file(char *pfile_name, char *pfile_data);

/*  Read data in a file
 *
 *   Returns > 0 - on success
 *   Returns < 0 - on error (error code)
 */
int zephyr_storage_read_file(char *pfile_name, char *pfile_data);

#endif //_ZEPHYRFILESYSTEM_H_