#include "mpns/types.h"
#include "mpns/code_generator.h"
#include <iostream>
#include <fstream>
#include <cstring>

#include "mpns/schedule.h"
#define INDENT ("    ")

namespace mpns {



static void generate_header_guard(std::ofstream& oss, std::string header) {
	oss << "#ifndef GUARD_" << header << "_H" << std::endl;
	oss << "#define GUARD_" << header << "_H" << std::endl << std::endl;
}

static void generate_identifiers(const char* directory, mpns_context* context) {
	// Generate declarations and enums for buffers
	char identifier_filename[128];
	strcpy(identifier_filename, directory);
	strcat(identifier_filename, "identifiers.h");
	
	std::ofstream iden_file;
	iden_file.open(identifier_filename);
	
	generate_header_guard(iden_file, "IDENTIFIERS");	
	
	iden_file << "enum buffer_identifiers {" << std::endl;	
	for (auto iter = context->buffer_map.begin(); iter != context->buffer_map.end(); iter++) {
		iden_file << INDENT << "BUFFER_" << iter->first->block_var->var_name << ", " << std::endl;
	}
	iden_file << "};" << std::endl;

	iden_file << "enum function_identifiers {" << std::endl;	
	for (auto iter = context->function_map.begin(); iter != context->function_map.end(); iter++) {
		iden_file << INDENT << "FUNCTION_" << iter->first->block_var->var_name << ", " << std::endl;
	}
	iden_file << "};" << std::endl;
	
	iden_file << std::endl << "#endif" << std::endl;

	iden_file.close();
}


static void generate_declarations(const char* directory, mpns_context* context, device* this_device) {
	char decl_filename[128];
	strcpy(decl_filename, directory);
	strcat(decl_filename, "declarations.h");
	
	std::ofstream decl_file;
	decl_file.open(decl_filename);
	
	generate_header_guard(decl_file, "DECLARATIONS");
	
	block::c_code_generator gen (decl_file);	
	gen.curr_indent = 0;
	// Generate buffer declarations
	for (auto iter = context->buffer_map.begin(); iter != context->buffer_map.end(); iter++) {
		block::decl_stmt::Ptr decl = std::make_shared<block::decl_stmt>();
		decl->decl_var = iter->first->block_var;
		decl_file << "extern ";
		if (iter->second == this_device) {
			decl->accept(&gen);
		} else {
			std::string old_name = decl->decl_var->var_name;
			decl->decl_var->var_name = "__local_" + old_name;
			decl->accept(&gen);
			decl->decl_var->var_name = old_name;
		}
		decl_file << std::endl;	
	}
	decl_file.close();
}

void device::generate_code(const char* directory) {

	// Post-extraction transformations passes to 
	// apply scheduling 
	

	generate_identifiers(directory, context);
	generate_declarations(directory, context, this);

	
	
	buffer_access_transform transform(context, this);
	for (auto function: mapped_functions) {
		transform.applyOnFunction(context->function_map[function].second);
	}			



	// Finally generate functions
	char function_filename[128];	
	strcpy(function_filename, directory);
	strcat(function_filename, "/functions.c");

	std::ofstream func_file;
	func_file.open(function_filename);
	func_file << "#include \"identifiers.h\"" << std::endl;	
	func_file << "#include \"declarations.h\"" << std::endl;	
	func_file << "#include \"mpns_runtime.h\"" << std::endl;
	

	block::c_code_generator gen (func_file);	
	for (auto buf: mapped_buffers) {
		block::decl_stmt::Ptr decl = std::make_shared<block::decl_stmt>();
		decl->decl_var = buf->block_var;
		decl->accept(&gen);	
		func_file << std::endl;
	}
	for (auto function: mapped_functions) {
		block::block::Ptr function_impl = context->function_map[function].second;
		block::c_code_generator::generate_code(function_impl, func_file, 0);
	} 	

	func_file << "void mpns_device_init(void) {" << std::endl;
	for (auto iter = context->buffer_map.begin(); iter != context->buffer_map.end(); iter++) {
		func_file << INDENT << "mpns_register_buffer_";
		if (iter->second == this) 
			func_file << "local(";
		else
			func_file << "remote(";
		func_file << "BUFFER_" << iter->first->block_var->var_name << ", ";
		if (iter->second == this)
			func_file << iter->first->block_var->var_name;
		else
			func_file << "__local_" << iter->first->block_var->var_name;
		func_file << ");" << std::endl;
		
	}
	func_file << "}" << std::endl;

	func_file << "static char* mpns_local_table[MAX_BUFFER_LIMIT];" << std::endl;
	
	func_file.close();
	
	
}

}
