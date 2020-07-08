#include "key_value.h"

int main(int argc, char* argv[]) {
	
	// Declare context and devices
	mpns::mpns_context context;
	
	mpns::device server(context), client(context);	

	// Configure device properties and interconnects
	// TODO: Add support for configuring connections between devices

	// Map buffers to devices: This is where the current magic lies
	context.map_buffer(&key_list, &server);
	context.map_buffer(&key_to_value, &server);
	context.map_buffer(&total_keys, &server);
	
	context.map_buffer(&value_list, &client);

	// Map functions and their implementations to devices
	// In the future, support ways to map functions without supplying implementations
	context.map_function_implementation(&retrieve_key, &server, builder::builder_context().extract_function_ast(retrieve_key_impl, "retrieve_key"));
	context.map_function_implementation(&retrieve_value, &client, builder::builder_context().extract_function_ast(retrieve_value_impl, "retrieve_value"));
	context.map_function_implementation(&search, &client, builder::builder_context().extract_function_ast(search_impl, "search"));

	// Unmapped functions like strcmp_f get automatically mapped to call site
	// This can be made explicit with a map_any function
	// Unmapped functions can have implementations or be extern
	

	// Buffer and Function naming
	// TODO: Take care of this inside the buffer type itself
	key_list.block_var->var_name = "key_list";
	key_to_value.block_var->var_name = "key_to_value";
	value_list.block_var->var_name = "value_list";
	total_keys.block_var->var_name = "total_keys";

	retrieve_key.block_var->var_name = "retreive_key";
	retrieve_value.block_var->var_name = "retrieve_value";
	search.block_var->var_name = "search";
	strcmp_f.block_var->var_name = "strcmp";


	
	// Finally generate code for a subset of devices
	client.generate_code(argv[1]);
	
}
