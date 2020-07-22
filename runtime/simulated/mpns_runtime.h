#ifndef MPNS_RUNTIME_H
#define MPNS_RUNTIME_H

#define MAX_BUFFER_LIMIT 1024

void mpns_sync_buffer_in(int buffer_id, int start, int size);
void mpns_sync_buffer_out(int buffer_id, int start, int size);

extern char* mpns_home_table[MAX_BUFFER_LIMIT];

static void mpns_register_buffer_remote(int buffer_id, void* buffer) {
	mpns_local_table[buffer_id] = buffer;
}
extern void mpns_register_buffer_local(int buffer_id, void* buffer);
static void mpns_sync_buffer_in(int buffer_id, int start, int size) {
	char* src_address = mpns_home_table[buffer_id];
	char* dst_address = mpns_local_table[buffer_id];

	// Simulate a blocking copy over the network	
	memcpy(dst_address + start, src_address + start + size);
}

static void mpns_sync_buffer_out(int buffer_id, int start, int size) {
	char* dst_address = mpns_home_table[buffer_id];
	char* src_address = mpns_local_table[buffer_id];

	// Simulate a blocking copy over the network	
	memcpy(dst_address + start, src_address + start + size);
}

#endif
