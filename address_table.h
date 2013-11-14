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

#ifndef ADDRESS_TABLE_H_
#define ADDRESS_TABLE_H_

#include <stddef.h>    //size_t
#include <inttypes.h>  //uintptr_t

typedef struct node node_t;

/** Statistics per unique address */
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

/** Total memory access statistics */
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
