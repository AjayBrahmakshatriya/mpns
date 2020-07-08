#ifndef MPNS_TYPES_H
#define MPNS_TYPES_H

#include <builder/builder_context.h>
#include <builder/builder.h>
#include <unordered_map>

namespace mpns {


class device;
class mpns_context {
public:
	std::unordered_map<builder::var*, device*> buffer_map;
	std::unordered_map<builder::var*, std::pair<device*, block::block::Ptr>> function_map;
	

	void map_buffer(builder::var*, device*);
	void map_function_implementation(builder::var*, device*, block::block::Ptr);
};


class device {
public:

	//optional
	std::string name;
	mpns_context* context;	
	std::vector<builder::var*> mapped_functions;
	std::vector<builder::var*> mapped_buffers;
	void generate_code(const char*);
	device(mpns_context& _context): context(&_context) {}
};



}

#endif
