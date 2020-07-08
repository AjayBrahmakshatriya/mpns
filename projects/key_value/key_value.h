#ifndef KEY_VALUE_H
#define KEY_VALUE_H

#include "mpns/types.h"
using builder::dyn_var;

typedef dyn_var<int> int_t;
typedef dyn_var<void*> void_t;
typedef dyn_var<char*> cstr_t;


// Buffer declarations
extern dyn_var<char**> key_list;
extern dyn_var<int*> key_to_value;
extern dyn_var<int> total_keys;
extern dyn_var<void**> value_list;


// Function declarations
extern dyn_var<int(char*)> retrieve_key;
extern dyn_var<void* (int)> retrieve_value;
extern dyn_var<void* (char*)> search;

// Functions for update



// Function implementations
extern int_t retrieve_key_impl(cstr_t); 
extern void_t retrieve_value_impl(int_t);
extern void_t search_impl(cstr_t);



// Library implementations without implementation
extern dyn_var<int (char*, char*)> strcmp_f;
#endif
