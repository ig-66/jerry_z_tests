#ifndef _ZEPHYRSTRINGCONVERSION_H_
#define _ZEPHYRSTRINGCONVERSION_H_

#include "jerryscript.h"

jerry_value_t
atob_handler(const jerry_call_info_t *call_info_p,
				const jerry_value_t arguments[],
				const jerry_length_t arguments_count);

jerry_value_t
btoa_handler(const jerry_call_info_t *call_info_p,
				const jerry_value_t arguments[],
				const jerry_length_t arguments_count);

#endif //_ZEPHYRSTRINGCONVERSION_H_