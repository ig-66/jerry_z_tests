#include "ZephyrHash.h"

#ifndef CONFIG_TINYCRYPT_SHA256
#error "JZ Library Dependency Error: TinyCrypt SHA-256 is not enabled"
#endif


void digest(char *algorithm, uint8_t *digest, char *message);

void digest_sha256(uint8_t *digest, char *message);


jerry_value_t
digest_handler(const jerry_call_info_t *call_info_p,
			   const jerry_value_t arguments[],
			   const jerry_length_t arguments_count)
{	
	/* WARNING: Only SHA-256 is implemented, so the 
	*	algorithm argument is ignored
	*/
	if(arguments_count != 2){
		return jerry_undefined();
	}
	jerry_value_t algorithm_value = jerry_value_to_string(arguments[0]);
	jerry_char_t algorithm_buffer[8]; 

	jerry_size_t algorithm_len = jerry_string_to_buffer(algorithm_value, JERRY_ENCODING_UTF8, algorithm_buffer, sizeof(algorithm_buffer) - 1);
	algorithm_buffer[algorithm_len] = '\0';

	jerry_value_free(algorithm_value);

	if(algorithm_len != 7){
		// handle error
		return jerry_undefined();
	}

	jerry_value_t message_value = jerry_value_to_string(arguments[1]);

	jerry_char_t message_buffer[1024]; 

	jerry_size_t message_len = jerry_string_to_buffer(message_value, JERRY_ENCODING_UTF8, message_buffer, sizeof(message_buffer) - 1);
	message_buffer[message_len] = '\0';
	jerry_value_free(message_value);
	
	char message[1024];

	memcpy(message, message_buffer, strlen(message_buffer)+1);

	uint8_t digested_len = 65;

	/* SHA-256 */
	if(strcmp("SHA-256", algorithm_buffer) == 0){
		digested_len = 65;
	}
	/* SHA-1 */
	if (strcmp("SHA-1", algorithm_buffer) == 0){
		digested_len = 41;
	}
	/* SHA-384 */
	if (strcmp("SHA-384", algorithm_buffer) == 0){
		digested_len = 97;
	}
	/* SHA-512 */
	if (strcmp("SHA-512", algorithm_buffer) == 0){
		digested_len = 129;
	}

	uint8_t digested_message[digested_len];
	digest(algorithm_buffer, digested_message, message);

	return jerry_string_sz(digested_message);
}


void digest(char *algorithm, uint8_t *digest, char *message){
	/* SHA-256 */
	if(strcmp("SHA-256", algorithm) == 0){
		digest_sha256(digest, message);
	}
	
	/* SHA-1 */
	if (strcmp("SHA-1", algorithm) == 0){
		// SOMETIME
	}	
	/* SHA-384 */
	if (strcmp("SHA-384", algorithm) == 0){
		// SOMETIME
	}
	
	/* SHA-512 */
	if (strcmp("SHA-512", algorithm) == 0){
		// SOMETIME
	}
}

void digest_sha256(uint8_t *digest, char *message){
	struct tc_sha256_state_struct s;

	uint8_t digest_p[32]; 
	uint8_t digest_t[65];

	(void)tc_sha256_init(&s);
	tc_sha256_update(&s, (const uint8_t *)message, strlen(message));
	(void)tc_sha256_final(digest_p, &s);

	for (uint8_t i = 0; i < 32; i++)
	{
		sprintf(&digest_t[i*2], "%02x", digest_p[i]);
	}

	memcpy(digest, digest_t, 64U);
	digest[64] = '\0';

}