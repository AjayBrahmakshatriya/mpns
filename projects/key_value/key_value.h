#ifndef KEY_VALUE_H
#define KEY_VALUE_H

#include "mpns/types.h"
using builder::dyn_var;

typedef dyn_var<int> int_t;
typedef dyn_var<void*> void_t;
typedef dyn_var<char*> cstr_t;


// Buffer declarations
extern mpns::buffer_t<char[128]> key_list;
extern mpns::buffer_t<int[128]> key_to_value;
extern mpns::buffer_t<int> total_keys;
extern mpns::buffer_t<void**> value_list;


// Function declarations
extern mpns::function_t<int(char*)> retrieve_key;
extern mpns::function_t<void* (int)> retrieve_value;
extern mpns::function_t<void* (char*)> search;

// Functions for update



// Function implementations
extern int_t retrieve_key_impl(cstr_t); 
extern void_t retrieve_value_impl(int_t);
extern void_t search_impl(cstr_t);



// Library implementations without implementation
extern mpns::function_t<int (char*, char*)> strcmp_f;
#endif
