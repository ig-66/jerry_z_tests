#ifndef _ZEPHYRHASH_H_
#define _ZEPHYRHASH_H_

#include <zephyr/zephyr.h> 
#include <tinycrypt/sha256.h>
#include "jerryscript.h"
#include <string.h>
#include <stdio.h>

jerry_value_t
digest_handler(const jerry_call_info_t *call_info_p,
                const jerry_value_t arguments[],
                const jerry_length_t arguments_count);


#endif //_ZEPHYRHASH_H_