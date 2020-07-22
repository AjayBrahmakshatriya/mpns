#ifndef VALUE_STORE_H
#define VALUE_STORE_H

#include "mpns/types.h"
using builder::dyn_var;
typedef dyn_var<int> int_t;
typedef dyn_var<void*> void_t;
typedef dyn_var<char*> cstr_t;

// buffer declarations
extern mpns::buffer_t<int[1024]> values;
extern mpns::buffer_t<int> total_values;

// function declarations
extern mpns::function_t<int(int)> get_position;
extern mpns::function_t<int(int)> find_value;

extern mpns::function_t<void(int)> insert_value;
extern mpns::function_t<void(int, int)> update_value;
extern mpns::function_t<void(void)> sort_values;

// function implementation declarations
extern int_t get_position_impl(int_t);
extern int_t find_value_impl(int_t);
extern void insert_value_impl(int_t);
extern void update_value_impl(int_t, int_t);
extern void sort_values_impl(void);


#endif
