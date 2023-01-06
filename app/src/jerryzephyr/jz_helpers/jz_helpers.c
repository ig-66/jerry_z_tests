#include "jz_helpers.h"

#include "jerryscript.h"
#include "jerryscript-ext/properties.h"


int jz_object_create(char *obj_name_p){
    int error = 0;

    jerry_value_t new_obj = jerry_object();
    jerry_value_t global_obj = jerry_current_realm();
    jerry_value_t new_obj_name = jerry_string_sz(obj_name_p);
    jerry_value_t set_new_obj_res = jerry_object_set(global_obj, new_obj_name, new_obj);

    if (jerry_value_is_exception(set_new_obj_res))
    {
        error = -1;
    }

    jerry_value_free(new_obj);
    jerry_value_free(global_obj);
    jerry_value_free(new_obj_name);
    jerry_value_free(set_new_obj_res);

    return error;
}


int jz_object_add_prop(char *obj_name_p, char *prop_name_p, jerry_external_handler_t prop_handler_p){
    int error = 0;

    jerry_value_t global_obj = jerry_current_realm();
    jerry_value_t obj_name = jerry_string_sz(obj_name_p);
    jerry_value_t obj_ref = jerry_object_get(global_obj, obj_name);

    if(jerry_value_is_error(obj_ref)){
        error = -1;
    } else {
        
        jerry_value_t prop_string = jerry_string_sz(prop_name_p);
        jerry_value_t prop_handler = jerry_function_external(prop_handler_p);
        jerry_value_t set_new_prop_res = jerry_object_set(obj_ref, prop_string, prop_handler);

        if (jerry_value_is_exception(set_new_prop_res)) {
            error = -1;
        }

        jerry_value_free(prop_string);
        jerry_value_free(prop_handler);
        jerry_value_free(set_new_prop_res);
    }

    jerry_value_free(global_obj);
    jerry_value_free(obj_name);
    jerry_value_free(obj_ref);

    return error;
}


int jz_function_create(char *func_name_p, jerry_external_handler_t func_handler_p){
    int error = 0;

    jerry_value_t global_obj = jerry_current_realm();
    jerry_value_t func_name = jerry_string_sz(func_name_p);
    jerry_value_t func_value = jerry_function_external(func_handler_p);
    jerry_value_t set_func_res = jerry_object_set(global_obj, func_name, func_value);

    if (jerry_value_is_exception(set_func_res)) {
        error = -1;
    }

    jerry_value_free(global_obj);
    jerry_value_free(func_name);
    jerry_value_free(func_value);
    jerry_value_free(set_func_res);

    return error;
}