#include "key_value.h"

// Buffer declarations
dyn_var<char**> key_list(true);
dyn_var<int*> key_to_value(true);
dyn_var<int> total_keys(true);
dyn_var<void**> value_list(true);


// Function declarations
dyn_var<int(char*)> retrieve_key(true);
dyn_var<void* (int)> retrieve_value(true);
dyn_var<void* (char*)> search(true);

// Function implementations
int_t retrieve_key_impl(cstr_t key) {
	for (int_t index = 0; index < total_keys; index = index + 1) {
		if (strcmp_f(key, key_list[index])) {
			return key_to_value[index];
		}
	}
	return -1;
}

void_t retrieve_value_impl(int_t index) {
	if (index == -1)
		return 0;	
	return value_list[index];
}

void_t search_impl(cstr_t key) {
	int_t index = retrieve_key(key);
	return retrieve_value(index);
}



// Library functions declarations
dyn_var<int(char*, char*)> strcmp_f(true);
