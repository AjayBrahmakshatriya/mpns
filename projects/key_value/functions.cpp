#include "key_value.h"

// Buffer declarations
mpns::buffer_t<char**> key_list("key_list");
mpns::buffer_t<int*> key_to_value("key_to_value");
mpns::buffer_t<int> total_keys("total_keys");
mpns::buffer_t<void**> value_list("value_list");


// Function declarations
mpns::function_t<int(char*)> retrieve_key("retrieve_key");
mpns::function_t<void* (int)> retrieve_value("retrieve_value");
mpns::function_t<void* (char*)> search("search");

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
mpns::function_t<int(char*, char*)> strcmp_f("strcmp");
