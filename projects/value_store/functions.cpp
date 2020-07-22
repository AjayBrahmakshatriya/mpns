#include "value_store.h"

// buffer declarations
mpns::buffer_t<int[1024]> values("values");
mpns::buffer_t<int> total_values("total_values");

mpns::function_t<int(int)> get_position("get_position");
mpns::function_t<int(int)> find_value("find_value");
mpns::function_t<void(int)> insert_value("insert_value");
mpns::function_t<void(int, int)> update_value("update_value");
mpns::function_t<void(void)> sort_values("sort_values");


int_t get_position_impl(int_t pos) {
	if (pos < total_values)
		return values[pos];
	return -1;
}

int_t find_value_impl(int_t val) {
	for (int_t i = 0; i < total_values; i=i+1)
		if (values[i] == val)
			return i;
	return -1;
}

void insert_value_impl(int_t val) {
	// TODO: figure out a way to overload/programmatically get sizeof buffer
	if (total_values == 1024) 
		return;
	values[total_values] = val;
	total_values = total_values + 1;
}

void update_value_impl(int_t pos, int_t val) {
	if (pos >= total_values)
		return;
	values[pos] = val;
}


void sort_values_impl(void) {
	for (int_t i = 0; i < total_values; i=i+1) {
		for (int_t j = i+1; j < total_values; j=j+1) {
			if (values[i] > values[j]) {
				int_t temp = values[i];
				values[i] = values[j];
				values[j] = temp;
			}
		}
	}
}
