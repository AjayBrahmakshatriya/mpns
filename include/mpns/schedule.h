#ifndef MPNS_SCHEDULE_H
#define MPNS_SCHEDULE_H

#include "blocks/block_visitor.h"
#include "blocks/block.h"
#include "blocks/expr.h"
#include "blocks/stmt.h"
#include "mpns/types.h"
#include <vector>
namespace mpns {
class buffer_access_transform {
public:
	class gather_read_write: public block::block_visitor {
	public:
		std::vector<block::var::Ptr> read_set;
		std::vector<block::var::Ptr> write_set;
	
		virtual void visit(block::assign_expr::Ptr);
		virtual void visit(block::var_expr::Ptr);
	
	
		mpns_context* context;
		gather_read_write(mpns_context *_context): context(_context) {}
	};

	class replace_buffer_vars: public block::block_visitor {
	public:
		mpns_context* context;
		device *this_device;
		replace_buffer_vars(mpns_context* _context, device* _this_device): context(_context), this_device(_this_device) {}
		
		virtual void visit(block::var_expr::Ptr);
	};
	

	device *this_device;
	mpns_context* context;

	buffer_access_transform(mpns_context* _context, device* _this_device): context(_context), this_device(_this_device) {}
	
	void applyOnFunction(block::block::Ptr);
	void process_block(block::stmt_block::Ptr block);

};
}
#endif
