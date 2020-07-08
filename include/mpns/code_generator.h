#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <blocks/c_code_generator.h>

class device_function_generator: public block::c_code_generator {
public:
	using c_code_generator::visit;
	device_function_generator(std::ostream &_oss): c_code_generator(oss) {}
};

#endif
