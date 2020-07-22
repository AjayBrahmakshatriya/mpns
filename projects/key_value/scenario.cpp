#include "key_value.h"

int main(int argc, char* argv[]) {
	
	// Declare context and devices
	mpns::mpns_context context;
	
	mpns::device server(context), client(context);	

	// Configure device properties and interconnects
	// TODO: Add support for configuring connections between devices

	// Map buffers to devices: This is where the current magic lies
	context.map_buffer(key_list, &server);
	context.map_buffer(key_to_value, &server);
	context.map_buffer(total_keys, &server);
	
	context.map_buffer(value_list, &server);

	// Map functions and their implementations to devices
	// In the future, support ways to map functions without supplying implementations
	//context.map_function_implementation(retrieve_key, &server, builder::builder_context().extract_function_ast(retrieve_key_impl, "retrieve_key"));
	//context.map_function_implementation(retrieve_value, &client, builder::builder_context().extract_function_ast(retrieve_value_impl, "retrieve_value"));
	//context.map_function_implementation(search, &client, builder::builder_context().extract_function_ast(search_impl, "search"));

	context.map_function_implementation(retrieve_key, &server, retrieve_key_impl);
	context.map_function_implementation(retrieve_value, &client, retrieve_value_impl);
	context.map_function_implementation(search, &client, search_impl);
	// Unmapped functions like strcmp_f get automatically mapped to call site
	// This can be made explicit with a map_any function
	// Unmapped functions can have implementations or be extern
	
	// Finally generate code for a subset of devices
	client.generate_code(argv[1]);
	
}
