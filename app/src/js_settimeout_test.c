
#include <zephyr/zephyr.h>

#include <string.h>
#include "jerryscript.h"
#include "jerryscript-ext/handlers.h"
#include "jerryscript-ext/properties.h"

/* 10000 msec = 10 sec */
#define SLEEP_TIME_MS   10000

#define JS_STACK_SIZE 4096
#define JS_PRIORITY 5

#define JS_TIME_STACK_SIZE 1024
#define JS_TIME_PRIORITY 10

/* MAIL BOX INITIALIZATION */
K_MBOX_DEFINE(js_mailbox);

struct js_func_schedule {
	jerry_value_t value;			// function value
	uint32_t time_interval;			// time in ms
	uint32_t time_counter;			// time until execution
	uint8_t type; 					// 0: setInterval, 1: setTimeout
	uint32_t timer_id;				// id of the timer, can be used to cancel the timer later on
	uint8_t queue;					// the queue position of the timer
};

typedef struct js_func_schedule js_func;

extern void jerry_start(void *, void *, void *);
extern void timeout_exec(void *, void *, void *);

void produce_thread_message(js_func script);
void consume_thread_message(js_func *buffer);

/* Start Jerry engine on a new thread */
K_THREAD_DEFINE(js_tid, JS_STACK_SIZE,
				jerry_start, NULL, NULL, NULL,
				JS_PRIORITY, 0, 0);


/* Start Jerry timeout on a new thread */
K_THREAD_DEFINE(js_time_id, JS_TIME_STACK_SIZE,
				timeout_exec, NULL, NULL, NULL,
				JS_TIME_PRIORITY, 0, 0);


int js_example_print_handler(void);

static jerry_value_t
timeout_handler(const jerry_call_info_t *call_info_p,
				const jerry_value_t arguments[],
				const jerry_length_t arguments_count);

static jerry_value_t
interval_handler(const jerry_call_info_t *call_info_p,
				const jerry_value_t arguments[],
				const jerry_length_t arguments_count);

void main(void){

	while (1) {
		k_msleep(SLEEP_TIME_MS);
		printk("PING from main()\n");
	}
}


void jerry_start(void * unused1, void * unused2, void * unused3){

	/* Init jerry engine */
	jerry_init(JERRY_INIT_EMPTY);

	/* Register print function */
	jerryx_register_global ("print", jerryx_handler_print);

	js_example_print_handler();

	while(1){
		// check for timers and execute them!
		k_msleep(1000);
	}
}


int js_example_print_handler (void) {
	
	const jerry_char_t script[] = "print('This is from the Jerry Thread!'); \
		print ('Hello from JS!'); \
		var number = 5; \
		print(number); \
		setTimeout(function(){ \
			print('older Number is: ' + number); \
			number = 10; \
			print('new Number is: ' + number); \
		}, 10000); \
		setInterval(function(){ \
			print('1 second interval'); \
		}, 1000); \
		print('After timeout in script'); \
	";

	const jerry_length_t script_size = sizeof (script) - 1;

	{
		/* Get the "global" object */
		jerry_value_t global_object = jerry_current_realm ();

		jerry_value_t property_name_timeout = jerry_string_sz ("setTimeout");
		/* Create a function from a native C method (this function will be called from JS) */
		jerry_value_t property_func_timeout = jerry_function_external (timeout_handler);
		/* Add the "setTimeout" property with the function value to the "global" object */
		jerry_value_t set_timeout_res = jerry_object_set(global_object, property_name_timeout, property_func_timeout);

		/* Check if there was no error when adding the property (in this case it should never happen) */
		if (jerry_value_is_exception(set_timeout_res)){
			printk("Failed to add the 'setTimeout' function property\n");
			return 1;
		}

		/* Release all jerry_value_t-s */
		jerry_value_free(property_name_timeout);
		jerry_value_free(property_func_timeout);
		jerry_value_free(set_timeout_res);

		jerry_value_t property_name_interval = jerry_string_sz("setInterval");
		/* Create a function from a native C method (this function will be called from JS) */
		jerry_value_t property_func_interval = jerry_function_external(interval_handler);
		/* Add the "setTimeout" property with the function value to the "global" object */
		jerry_value_t set_interval_res = jerry_object_set(global_object, property_name_interval, property_func_interval);

		/* Check if there was no error when adding the property (in this case it should never happen) */
		if (jerry_value_is_exception(set_interval_res)){
			printk ("Failed to add the 'setTimeout' function property\n");
			return 1;
		}

		/* Release all jerry_value_t-s */
		jerry_value_free (global_object);
		jerry_value_free (property_name_interval);
		jerry_value_free (property_func_interval);
		jerry_value_free (set_interval_res);
	}


	/* Setup Global scope code */
	jerry_value_t parsed_code = jerry_parse (script, script_size, NULL);

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

	return 0;
}


extern void timeout_exec(void * unused1, void * unused2, void * unused3){
	// printk(">> INSIDE TIMEOUT THREAD <<\n");
	printk(">\n");

	/******************************************************************************
		Get message from mail box, it should have the code do execute, time, 
		type (setTimeout or setInterval), and timer ID 
	******************************************************************************/
	// k_msleep(10000);
	js_func buffer;

	static js_func func_array[10];
	uint8_t func_array_sz = sizeof(func_array)/sizeof(func_array[0]);

	const uint8_t JS_TIME_STEP = 10;

	while(1){
		consume_thread_message(&buffer);
		
		/* Place the new function in the function array */
		if(buffer.value != 0){
			for(uint8_t i = 0; i < func_array_sz; i++){
				if(func_array[i].value == 0){
					func_array[i] = buffer;
					func_array[i].time_counter = buffer.time_interval;
					buffer.value = 0;
				}
			}
		}

		for(uint8_t i = 0; i < func_array_sz; i++){
			
			if(func_array[i].value != 0){
				/*
				printk("Function [%d]:\n  \
				  	Value: 		%d\n \
					Interval:	%d\n \
					Counter:	%d\n \
					Type:		%s\n \
				  \n", i, func_array[i].value, func_array[i].time_interval, func_array[i].time_counter,
				  func_array[i].type ? "setInterval" : "setTimeout");
				*/

				func_array[i].time_counter = func_array[i].time_counter - JS_TIME_STEP;

				if(func_array[i].time_counter <= 0){
					printk("Executing...\n");
		jerry_value_t undefined;
					jerry_value_t ret = jerry_call (func_array[i].value, undefined, NULL, 0);
					jerry_value_free(undefined);
					jerry_value_free(ret);
					
					if(func_array[i].type == 0){
						func_array[i].value = 0;
					} else {
						printk("reseting...\n");
						func_array[i].time_counter = func_array[i].time_interval;
					}
				}
			}
		}
		k_msleep(JS_TIME_STEP);
	}
}


static jerry_value_t
timeout_handler (const jerry_call_info_t *call_info_p,
			   const jerry_value_t arguments[],
			   const jerry_length_t arguments_count)
{
	/* There should be at least one argument */
	if (arguments_count > 0)
	{	
		// TODO:
		/* The setTimeout handler should also verify the type of the arguments being passed to it */
		
		if(jerry_value_is_function (arguments[0])){
			printk("It is a function\n");
			/**************************************************************
				should send the code to execute through mailbox, 
				asynchronously, so it does not await the message to be 
				received by the receiver
			**************************************************************/
			js_func function;
			
			function.value = arguments[0];
			function.time_interval = (arguments[1]/16);
			function.type = 0; // timeout

			produce_thread_message(function);
		} else {
			printk("-- ERROR: while setting the timeout, it is not a function!\n");
			return 1;
		}
		return 0;
	}
	return 0;
}
		

static jerry_value_t
interval_handler(const jerry_call_info_t *call_info_p,
				const jerry_value_t arguments[],
				const jerry_length_t arguments_count)
{
	/* There should be at least one argument */
	if (arguments_count > 0)
	{
		// TODO:
		/* The setTimeout handler should also verify the type of the arguments being passed to it */

		if (jerry_value_is_function(arguments[0])){
			/**************************************************************
				should send the code to execute through mailbox,
				asynchronously, so it does not await the message to be
				received by the receiver
			**************************************************************/
			js_func function;

			function.value = arguments[0];
			function.time_interval = (arguments[1] / 16);
			function.type = 1; // interval

			produce_thread_message(function);
		} else {
			printk("-- ERROR: while setting the interval, it is not a function!\n");
			return 1;
		}
		return 0;
	}

	return 0;
}


void produce_thread_message(js_func script)
{
	struct k_mbox_msg send_msg;

	// while (1) {
		js_func buffer;

		buffer.value = script.value;
		buffer.time_interval = script.time_interval;
		buffer.type = script.type; 

		int buffer_bytes_used = sizeof(js_func);
		// memcpy(buffer, script, buffer_bytes_used);

		/* prepare to send message */
		send_msg.info = buffer_bytes_used;
		send_msg.size = buffer_bytes_used;
		send_msg.tx_data = &buffer;
		send_msg.tx_block.data = NULL;
		send_msg.tx_target_thread = K_ANY;

		/* send message and wait until a consumer receives it */
		// printk("-- Sending message...\n");
		k_mbox_put(&js_mailbox, &send_msg, K_MSEC(1000));
		// printk("-- Message sent!\n");
		/* info, size, and tx_target_thread fields have been updated */

		/* verify that message data was fully received */
		if (send_msg.size < buffer_bytes_used) {
			printk("some message data dropped during transfer!");
			printk("receiver only had room for %d bytes", send_msg.info);
		}
	// }
}


void consume_thread_message(js_func * buffer)
{
	struct k_mbox_msg recv_msg;

	// while (1) {
		/* prepare to receive message */
		recv_msg.size = sizeof(js_func);
		recv_msg.rx_source_thread = K_ANY;

		/* get message, but not its data */
		k_mbox_get(&js_mailbox, &recv_msg, NULL, K_NO_WAIT);

		/* retrieve message data and delete the message */
		if (recv_msg.info == recv_msg.size){
		k_mbox_data_get(&recv_msg, buffer);
		} else {
			buffer->value = 0;
		}

	// }
}