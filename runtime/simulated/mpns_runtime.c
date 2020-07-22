#include "mpns_runtime.h"

char* mpns_home_table[MAX_BUFFER_LIMIT];


extern void mpns_register_buffer_local(int buffer_id, void* buffer) {
	mpns_home_table[buffer_id] = buffer;
}
