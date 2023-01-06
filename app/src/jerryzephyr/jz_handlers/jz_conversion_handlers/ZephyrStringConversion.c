#include "ZephyrStringConversion.h"

#include <zephyr/zephyr.h>
#include <sys/base64.h>
#include <string.h>
#include <stdio.h>

#ifndef CONFIG_BASE64
#error "JZ Library Dependency Error: Base64 support is not enabled"
#endif

jerry_value_t
atob_handler(const jerry_call_info_t *call_info_p,
			 const jerry_value_t arguments[],
			 const jerry_length_t arguments_count)
{
	if(arguments_count != 1){
		return jerry_undefined();
	}

	size_t decoded_len;

	jerry_value_t string_value = jerry_value_to_string(arguments[0]);

	jerry_char_t buffer[685]; //enough for 512 bytes of data

	jerry_size_t b64_len = jerry_string_to_buffer(string_value, JERRY_ENCODING_UTF8, buffer, sizeof(buffer) - 1);
	buffer[b64_len] = '\0';

	jerry_value_free(string_value);

	unsigned int ascii_len = 0;

	if( (b64_len % 4) != 0){
		// error, not a base64 string
		return jerry_undefined();
	}

	if (b64_len > 5) {
		ascii_len = ( (b64_len / 4) * 3 ) + 1;
	} else {
		ascii_len = 4;
	}

	unsigned char decoded_string[ascii_len];

	if (!(base64_decode(decoded_string, ascii_len, &decoded_len, buffer, strlen(buffer)) == 0)) {
		// handle the error
	}
	decoded_string[decoded_len] = '\0';

	return jerry_string_sz(decoded_string);
}

jerry_value_t
btoa_handler(const jerry_call_info_t *call_info_p,
			 const jerry_value_t arguments[],
			 const jerry_length_t arguments_count)
{
	if(arguments_count != 1){
		return jerry_undefined();
	}

	size_t encoded_len;

	jerry_value_t string_value = jerry_value_to_string(arguments[0]);

	jerry_char_t buffer[513];

	jerry_size_t ascii_len = jerry_string_to_buffer(string_value, JERRY_ENCODING_UTF8, buffer, sizeof(buffer) - 1);
	buffer[ascii_len] = '\0';

	jerry_value_free(string_value);

	unsigned int b64_len = 0;
	
	if (ascii_len > 3) {
		b64_len = ((ascii_len / 3) + 1) * 4;
	} else {
		b64_len = 4;
	}
	
	b64_len++;

	unsigned char encoded_string[b64_len];

	if (!(base64_encode(encoded_string, b64_len, &encoded_len, buffer, strlen(buffer)) == 0)) {
		// handle the error
	}

	return jerry_string_sz(encoded_string);
}