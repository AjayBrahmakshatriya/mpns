#include "mpns/types.h"

namespace mpns {
void mpns_context::map_buffer(builder::var* buffer, device* dev) {
	buffer_map[buffer] = dev;
	dev->mapped_buffers.push_back(buffer);
}


void mpns_context::map_function_implementation(builder::var* function, device* dev, block::block::Ptr func_impl) {
	dev->mapped_functions.push_back(function);
	function_map[function] = std::make_pair(dev, func_impl);
}
}
