
#include "value_store.h"

int main(int argc, char* argv[]) {
	
	// Declare context and devices
	mpns::mpns_context context;
	
	mpns::device server(context), client(context);	

	// Configure device properties and interconnects
	// TODO: Add support for configuring connections between devices

	// Map buffers to devices: This is where the current magic lies
	context.map_buffer(values, &server);
	context.map_buffer(total_values, &server);
	

	// Map functions and their implementations to devices
	// In the future, support ways to map functions without supplying implementations
	context.map_function_implementation(get_position, &client, get_position_impl);
	context.map_function_implementation(find_value, &client, find_value_impl);
	context.map_function_implementation(insert_value, &client, insert_value_impl);
	context.map_function_implementation(update_value, &client, update_value_impl);
	context.map_function_implementation(sort_values, &client, sort_values_impl);

	
	// Finally generate code for a subset of devices
	client.generate_code(argv[1]);
	server.generate_code(argv[2]);
	
}
