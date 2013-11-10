
#ifndef ADDRESS_TABLE_H_
#define ADDRESS_TABLE_H_

#include <stddef.h>    //size_t
#include <inttypes.h>  //uintptr_t

typedef struct node node_t;
struct node {
	node_t *next;
	uintptr_t address;
	size_t last_read;
	size_t last_write;
	size_t reads;
	size_t writes;
	unsigned long long total_reuse_time_sum;
	unsigned long long read_reuse_time_sum;
	unsigned long long write_reuse_time_sum;
};

typedef struct {
	node_t **table;
	size_t table_size;
	unsigned long long distance_sum;
	size_t total_accesses;
	size_t count;
	uintptr_t last_access;
} address_table_t;

int address_table_init(address_table_t *table);
void address_table_clear(address_table_t *table);
void address_table_fini(address_table_t *table);

int address_table_record_read(address_table_t *table, uintptr_t address);
int address_table_record_write(address_table_t *table, uintptr_t address);




#endif
