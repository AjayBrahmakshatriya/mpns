#ifndef MPNS_TYPES_H
#define MPNS_TYPES_H

#include <builder/builder_context.h>
#include <builder/builder.h>
#include <unordered_map>

namespace mpns {

template <typename BT>
class buffer_t;

template <typename BT>
using function_t = buffer_t<BT>;

class device;
class mpns_context {
public:
	std::unordered_map<builder::var*, device*> buffer_map;
	std::unordered_map<builder::var*, std::pair<device*, block::block::Ptr>> function_map;

	template<typename BT>
	void map_buffer(buffer_t<BT>& buffer, device* dev);

	template<typename BT, typename FT>	
	void map_function_implementation(function_t<BT>&, device*, FT func);
};




/* The implementation for buffer_t type
Buffers are wrappers around dyn_var type variables */


template <typename BT>
class buffer_t {
public:
	builder::dyn_var<BT> *d_var = nullptr;
	// TODO: static asserts for type checking on BT
	// Template magic goes brrrr
	typedef builder::dyn_var<BT> wrapped_type;	

	// Operator implementation to expose the underlying dyn_var functionality
	operator builder::dyn_var<BT>&() {
		return *d_var;
	}
	operator const builder::dyn_var<BT>&() const {
		return *d_var;
	}

	operator builder::builder () {
		return (builder::builder)*d_var;
	}
	operator builder::builder () const {
		return (builder::builder)*d_var;
	}
	
	// This is a special case of an operator that needs to be defined
	// Otherwise the default assignment is called
	builder::builder operator =(const builder::dyn_var<BT>& t) const {
		return (*d_var = t);
	}

	// [] operator not being called directly after conversion
	builder::builder operator [] (const builder::builder t) const {
		return (*d_var)[t];
	}
	builder::builder operator == (const builder::builder t) const {
		return (*d_var == t);
	}
	builder::builder operator + (const builder::builder t) const {
		return (*d_var + t);
	}
	template<typename ...ArgTypes>
	builder::builder operator () (ArgTypes...args) const {
		return (*d_var)(args...);
	}
	// Constructors for named buffers declared at global scope
	buffer_t (std::string name) {
		d_var = new wrapped_type(true);
		d_var->block_var->var_name = name;
	}

	template <typename OBT>
	buffer_t (buffer_t<OBT>&) = delete;

	~buffer_t() {
		// TODO: free up the dyn_var resources. Merely deleting the dyn_var 
		// might be fine for now
		// Rule of 3 not being followed correctly
		delete d_var;
	}
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


template<typename BT>
void mpns_context::map_buffer(buffer_t<BT>& buffer, device* dev) {
	buffer_map[buffer.d_var] = dev;
	dev->mapped_buffers.push_back(buffer.d_var);
}
template<typename BT, typename FT>
void mpns_context::map_function_implementation(function_t<BT>& function, device* dev, FT func) {
	block::block::Ptr func_impl = builder::builder_context().extract_function_ast(func, function.d_var->block_var->var_name);
	dev->mapped_functions.push_back(function.d_var);
	function_map[function.d_var] = std::make_pair(dev, func_impl);
}

}

#endif
