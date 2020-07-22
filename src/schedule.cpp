#include "mpns/schedule.h"

namespace mpns {

	static bool is_buffer(block::var::Ptr var, mpns_context* context) {
		if (!(var->static_offset.pointers.size() == 0 && var->static_offset.static_var_snapshots.size() == 0))
			return false;
		for (auto iter = context->buffer_map.begin(); iter != context->buffer_map.end(); iter++)
			if (iter->first->block_var->var_name == var->var_name) {
				return true;
			}
		return false;
	}

	static bool is_local_buffer(block::var::Ptr var, mpns_context* context, device* this_device) {
		for (auto iter = context->buffer_map.begin(); iter != context->buffer_map.end(); iter++) {
			if (iter->first->block_var == var && iter->second == this_device)
				return true;
			else 
				return false;
		}
		assert(false && "Buffer not found");	
	}


	static block::stmt::Ptr create_sync(block::var::Ptr buffer, block::expr::Ptr index, block::expr::Ptr size, std::string type) {
		block::var_expr::Ptr arg1 = std::make_shared<block::var_expr>();
		arg1->var1 = std::make_shared<block::var>();
		arg1->var1->var_name = "BUFFER_" + buffer->var_name;
		
		block::function_call_expr::Ptr call = std::make_shared<block::function_call_expr>();
		block::var_expr::Ptr fname = std::make_shared<block::var_expr>();
		fname->var1 = std::make_shared<block::var>();
		fname->var1->var_name = "mpns_sync_buffer_" + type;
		
		call->expr1 = fname;
		call->args.push_back(arg1);
		call->args.push_back(index);
		call->args.push_back(size);

		block::expr_stmt::Ptr stmt = std::make_shared<block::expr_stmt>();
		stmt->expr1 = call;
		
		return stmt;
	}
	static block::stmt::Ptr create_sync_in(block::var::Ptr buffer, block::expr::Ptr index, block::expr::Ptr size) {
		return create_sync(buffer, index, size, "in");
	}
	static block::stmt::Ptr create_sync_out(block::var::Ptr buffer, block::expr::Ptr index, block::expr::Ptr size) {
		return create_sync(buffer, index, size, "out");
	}
	static block::expr::Ptr create_int_constant(int c) {
		block::int_const::Ptr con = std::make_shared<block::int_const>();
		con->value = c;
		return con;
	}
	static block::expr::Ptr create_size_expr(block::var::Ptr buffer) {
		block::var_expr::Ptr var_expr = std::make_shared<block::var_expr>();
		var_expr->var1 = std::make_shared<block::var>();
		var_expr->var1->var_name = "sizeof(";
		var_expr->var1->var_name += buffer->var_name;
		var_expr->var1->var_name += ")";
		return var_expr;
	}
	void buffer_access_transform::process_block(block::stmt_block::Ptr block) {	
		std::vector<block::stmt::Ptr> new_stmts;	
		for (auto stmt: block->stmts) {
			if (block::isa<block::for_stmt>(stmt)) {
				// Special case for for loop
				block::for_stmt::Ptr for_stmt = block::to<block::for_stmt>(stmt);
			
				// Handle the initialization
				buffer_access_transform::gather_read_write decl_visitor(context);
				for_stmt->decl_stmt->accept(&decl_visitor);
				
				// Handle condition
				buffer_access_transform::gather_read_write cond_visitor(context);
				for_stmt->cond->accept(&cond_visitor);

				// Handle update 
				buffer_access_transform::gather_read_write update_visitor(context);
				for_stmt->update->accept(&update_visitor);
				
				block::stmt_block::Ptr body_block = block::to<block::stmt_block>(for_stmt->body);
				process_block(body_block);
				
				// insert the reads for decl and condition before the loop
				for (auto v: decl_visitor.read_set) {
					if (!is_local_buffer(v, context, this_device))	
						new_stmts.push_back(create_sync_in(v, create_int_constant(0), create_size_expr(v)));
				}
				for (auto v: cond_visitor.read_set) {
					if (!is_local_buffer(v, context, this_device))	
						new_stmts.push_back(create_sync_in(v, create_int_constant(0), create_size_expr(v)));
				}

				// insert write set of condition in the beginning of the body
				for (auto v: cond_visitor.write_set) {
					if (!is_local_buffer(v, context, this_device))	
						body_block->stmts.insert(body_block->stmts.begin(), create_sync_in(v, create_int_constant(0), create_size_expr(v)));
				}
				// insert write set of update in the beginning of the body
				for (auto v: update_visitor.write_set) {
					if (!is_local_buffer(v, context, this_device))	
						body_block->stmts.insert(body_block->stmts.begin(), create_sync_in(v, create_int_constant(0), create_size_expr(v)));
				}

				
				// insert read set of condition and update at the end of the body
				for (auto v: update_visitor.read_set) {
					if (!is_local_buffer(v, context, this_device))	
						body_block->stmts.push_back(create_sync_in(v, create_int_constant(0), create_size_expr(v)));
				}
				for (auto v: cond_visitor.read_set) {
					if (!is_local_buffer(v, context, this_device))	
						body_block->stmts.push_back(create_sync_in(v, create_int_constant(0), create_size_expr(v)));
				}
				
				// insert the loop
				new_stmts.push_back(for_stmt);
			
				// Finally insert write set of condition	
				for (auto v: cond_visitor.write_set) {
					if (!is_local_buffer(v, context, this_device))	
						new_stmts.insert(body_block->stmts.begin(), create_sync_in(v, create_int_constant(0), create_size_expr(v)));
				}
					
						
			} else if (block::isa<block::while_stmt>(stmt)) {
				// TODO: Special case for while loop
			} else if (block::isa<block::if_stmt>(stmt)) {
				block::if_stmt::Ptr if_stmt = block::to<block::if_stmt>(stmt);
				
				buffer_access_transform::gather_read_write visitor(context);
				if_stmt->cond->accept(&visitor);
				
				for (auto v: visitor.read_set) {
					if (!is_local_buffer(v, context, this_device))	
						new_stmts.push_back(create_sync_in(v, create_int_constant(0), create_size_expr(v)));
				}
				process_block(block::to<block::stmt_block>(if_stmt->then_stmt));
				process_block(block::to<block::stmt_block>(if_stmt->else_stmt));
				auto &then_stmts = block::to<block::stmt_block>(if_stmt->then_stmt)->stmts;
				auto &else_stmts = block::to<block::stmt_block>(if_stmt->else_stmt)->stmts;
				for (auto v: visitor.write_set) {
					if (!is_local_buffer(v, context, this_device)) {
						then_stmts.insert(then_stmts.begin(), create_sync_out(v, create_int_constant(0), create_size_expr(v))); 
						else_stmts.insert(then_stmts.begin(), create_sync_out(v, create_int_constant(0), create_size_expr(v))); 
					}
				}	
				new_stmts.push_back(if_stmt);
			} else {
				// Normal stmt	
				buffer_access_transform::gather_read_write visitor(context);
				stmt->accept(&visitor);
				for (auto v: visitor.read_set) {
					if (!is_local_buffer(v, context, this_device))
						new_stmts.push_back(create_sync_in(v, create_int_constant(0), create_size_expr(v)));
				}
				new_stmts.push_back(stmt);
				for (auto v: visitor.write_set) {
					if (!is_local_buffer(v, context, this_device))
						new_stmts.push_back(create_sync_out(v, create_int_constant(0), create_size_expr(v)));
				}
				
			}
		}
		block->stmts = new_stmts;
	}

	void buffer_access_transform::applyOnFunction(block::block::Ptr _func) {
		block::func_decl::Ptr func = block::to<block::func_decl>(_func);
		process_block(block::to<block::stmt_block>(func->body));
		
		replace_buffer_vars visitor(context, this_device);
		func->accept(&visitor);
	}	
	
	static void add_to_set(std::vector<block::var::Ptr> &set, block::var::Ptr v) {
		for (auto i: set) {
			if (i == v)
				return;
		}
		set.push_back(v);
	}
	void buffer_access_transform::gather_read_write::visit(block::var_expr::Ptr var) {
		if (is_buffer(var->var1, context))
			add_to_set(read_set, var->var1);
	}
	void buffer_access_transform::gather_read_write::visit(block::assign_expr::Ptr expr) {
		if (block::isa<block::var_expr>(expr->var1)) {
			block::var_expr::Ptr var_expr = block::to<block::var_expr>(expr->var1);
			if (is_buffer(var_expr->var1, context))
				add_to_set(write_set, var_expr->var1);
		} else if (block::isa<block::sq_bkt_expr>(expr->var1)) {
			block::sq_bkt_expr::Ptr sq_bkt = block::to<block::sq_bkt_expr>(expr->var1);
			// For now we will allow only var expr in sq_bkt_expr			
			block::var_expr::Ptr var_expr = block::to<block::var_expr>(sq_bkt->var_expr);
			if (is_buffer(var_expr->var1, context))
				add_to_set(write_set, var_expr->var1);
			sq_bkt->index->accept(this);
		} else {
			assert(false && "Invalid type of expression for LHS");
		}
		expr->expr1->accept(this);
	}

	void buffer_access_transform::replace_buffer_vars::visit(block::var_expr::Ptr var_expr) {
		if (is_buffer(var_expr->var1, context) && !is_local_buffer(var_expr->var1, context, this_device)) {
			block::var::Ptr new_var = std::make_shared<block::var>();
			new_var->var_type = var_expr->var1->var_type;
			new_var->var_name = "__local_" + var_expr->var1->var_name;
			var_expr->var1 = new_var;
		}
	}

}
