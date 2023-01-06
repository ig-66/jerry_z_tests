#ifndef _JZ_HELPERS_H_
#define _JZ_HELPERS_H_

#include "jerryscript.h"

/*  Create a new Object in the global realm
 *
 *  @param obj_name_p - char array
 *
 * @return 0 if successful, otherwise failed
 */
int jz_object_create(char *obj_name_p);

/*  Add a property or method to an existing object, should be called after jz_object_create
 *
 *  @param obj_name_p - char array
 *  @param prop_name_p - char array
 *  @param prop_handler_p - external function handler
 *
 * @return 0 if successful, otherwise failed
 */
int jz_object_add_prop(char *obj_name_p, char *prop_name_p, jerry_external_handler_t prop_handler_p);

/*  Create a new function for the JerryScript machine
 *
 *  @param func_name_p - name of the new function
 *  @param func_handler_p - external function handler
 * 
 * @return 0 if successful, otherwise failed
 */
int jz_function_create(char *func_name_p, jerry_external_handler_t func_handler_p);

#endif //_JZ_HELPERS_H_