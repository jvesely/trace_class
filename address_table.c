/*
 * Copyright (c) 2013 Jan Vesely
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "address_table.h"
#include <assert.h>
#include <errno.h>
#include <stdlib.h>  //calloc
#include <string.h>  //memset
#include <stdio.h>


#define MAX(a,b) ((a) > (b) ? (a) : (b))

enum {
	/* Random prime number, should be enough for reasonable performance */
	NICE_PRIME = 2000003,
};

/** Find existing, or create new node for the address */
static node_t * get_node(address_table_t *table, uintptr_t address)
{
	assert(table);

	/* search the table */
	node_t **row = &table->table[address % table->table_size];
	for (node_t *it = *row; it != NULL; it = it->next) {
		if (it->address == address)
			return it;
	}

	/* not found, create new */
	node_t *new_node = calloc(1, sizeof(node_t));
	if (new_node) {
		new_node->next = *row;
		new_node->address = address;
		*row = new_node;
		++table->count;
	}
	return new_node;
}

/** Set everything to zero, allocate memory */
int address_table_init(address_table_t *table)
{
	if (!table)
		return -EINVAL;
	memset(table, 0, sizeof(*table));
	table->table = calloc(NICE_PRIME, sizeof(node_t *));
	if (!table->table)
		return -ENOMEM;
	table->table_size = NICE_PRIME;
	return 0;
}

/** Empty the table */
void address_table_clear(address_table_t *table)
{
	assert(table);
	node_t **old = table->table;
	const size_t old_size = table->table_size;
	size_t max_row = 0;
	size_t deleted = 0;
	for (size_t row = 0; row < old_size; ++row) {
		size_t current = 0;
		while (old[row]) {
			node_t *item = old[row];
			old[row] = item->next;
			free(item);
			++current;
			++deleted;
		}
		if (current > max_row)
			max_row = current;
	}
#ifdef DEBUG
	fprintf(stderr, "Max elements per row: %zu, total elements %zu, total "
		"accesses %zu\n", max_row, deleted, table->total_accesses);
#endif
	memset(table, 0, sizeof(*table));
	table->table = old;
	table->table_size = old_size;
}

/** Empty the table, free memory */
void address_table_fini(address_table_t *table)
{
	if (table) {
		address_table_clear(table);
		free(table->table);
		memset(table, 0, sizeof(*table));
	}
}

/** Process statistics for memory access. */
static int record_access(address_table_t *table, uintptr_t address, int write)
{
	assert(table);
	/* Update distance from the last access */
	if (table->last_access)
		table->distance_sum += abs(address - table->last_access);
	table->last_access = address;
	++table->total_accesses;

	node_t *node = get_node(table, address);

	/* update node */
	const size_t last_use = MAX(node->last_read, node->last_write);
	if (last_use)
		node->total_reuse_time_sum += (table->total_accesses - last_use);
	if (write) {
		/* process write */
		++node->writes;
		if (node->last_write)
			node->write_reuse_time_sum +=
				(table->total_accesses - node->last_write);
		node->last_write = table->total_accesses;
	} else {
		/* process read */
		++node->reads;
		if (node->last_read)
			node->read_reuse_time_sum +=
				(table->total_accesses - node->last_read);
		node->last_read = table->total_accesses;
	}

	return 0;
}

/** Read access helper */
int address_table_record_read(address_table_t *table, uintptr_t address)
{
	return record_access(table, address, 0);
}

/** Write access helper */
int address_table_record_write(address_table_t *table, uintptr_t address)
{
	return record_access(table, address, 1);
}

