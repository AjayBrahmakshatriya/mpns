#include "mpns/types.h"
#include "mpns/code_generator.h"
#include <iostream>
#include <fstream>
#include <cstring>


namespace mpns {
void device::generate_code(const char* directory) {
	char function_filename[128];	
	strcpy(function_filename, directory);
	strcat(function_filename, "/functions.c");



	std::ofstream func_file;
	func_file.open(function_filename);
	device_function_generator generator(std::cout);
	generator.curr_indent = 0;
	
	for (auto function: mapped_functions) {
		block::block::Ptr function_impl = context->function_map[function].second;
		block::c_code_generator::generate_code(function_impl, func_file, 0);
		/*
		function_impl->accept(&generator);
		func_file << std::endl;
		*/
	} 

		
	func_file.close();
	
	
}

}
