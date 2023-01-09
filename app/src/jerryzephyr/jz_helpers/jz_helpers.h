#ifndef _JZ_HELPERS_H_
#define _JZ_HELPERS_H_

#include "jerryscript.h"

/**  Create a new Object in the global realm
 *
 *  @param obj_name_p string name of the new object, e.g. "console" 
 *
 * @return 0 if successful, otherwise failed
 */
int jz_object_create(char *obj_name_p);

/**  Add a property or method to an existing object, should be called after jz_object_create
 *
 *  @param obj_name_p string name of the object, e.g. "console"
 *  @param prop_name_p string name of the new prop, e.g. "log"
 *  @param prop_handler_p external function handler for methods, e.g. jerryx_handler_print or NULL for objects
 *
 *  @note Call jz_object_create function before this one
 *
 * @return 0 if successful, otherwise failed
 */
int jz_object_add_prop(char *obj_name_p, char *prop_name_p, jerry_external_handler_t prop_handler_p);

/**  Add an object to an existing object, should be called after jz_object_create
 *
 *  @param obj_name_p string name of the object, e.g. "crypto"
 *  @param inner_obj_name_p string name of the new prop, e.g. "subtle"
 *
 *  @note Call jz_object_create function before this one
 *
 * @return 0 if successful, otherwise failed
 */
int jz_object_add_obj(char *obj_name_p, char *inner_obj_name_p);

/** Add a new prop/method to a existing prop of an existing object
 *
 * @param obj_name_p string name of the object, e.g. "crypto"
 * @param prop_name_p string name of the existing prop, e.g. "subtle"
 * @param new_prop_name_p string name of the new prop, e.g. "digest"
 * @param prop_handler_p external function handler, e.g. digest_handler
 * 
 * @return 0 if successful, otherwise failed
 */
int jz_object_prop_add_prop(char *obj_name_p, char *prop_name_p, char *new_prop_name_p, jerry_external_handler_t prop_handler_p);

/**  Create a new function for the JerryScript machine
 *
 *  @param func_name_p string name of the new function, e.g. "atob"
 *  @param func_handler_p external function handler, e.g. atob_handler
 * 
 * @return 0 if successful, otherwise failed
 */
int jz_function_create(char *func_name_p, jerry_external_handler_t func_handler_p);

#endif //_JZ_HELPERS_H_