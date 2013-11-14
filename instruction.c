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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"

/* file format:
 * 1: inst addr, 2: inst category,
 * 3: src reg number, 4 ~ N: src regs
 * N+1: dest reg number, N+2 ~ M: dest regs
 * M+1: src mem number, M+2 ~ O: src mems
 * O+1: dest mem number, O+2 ~ P: dest mems
 * at least 8 fields
 */

/** Read one line and parse it to instruction structure. */
int instruction_get(instruction_t *insn, void* file, getline_t getline)
{
	if (!insn)
		return -ENOMEM;
	memset(insn, 0, sizeof(*insn));
	char tmp_buffer[1024];
	if (!getline(file, tmp_buffer, sizeof(tmp_buffer)))
		return -1;

	errno = 0;
	char *it = tmp_buffer;
	insn->code = strtoll(it, &it, 16);
	insn->category = strtoll(it + 1, &it, 10);

	const struct {unsigned *count; uintptr_t **place; int base; }
	places[] = {
		{&insn->src_reg_count, &insn->src_reg, 10},
		{&insn->dst_reg_count, &insn->dst_reg, 10},
		{&insn->src_count, &insn->src_mem, 16},
		{&insn->dst_count, &insn->dst_mem, 16},
	};
	for (unsigned i = 0;
	     errno == 0 && i < sizeof(places) / sizeof(places[0]); ++i) {
		*places[i].count = strtoll(it + 1, &it, 10);
		*places[i].place = calloc(*places[i].count, sizeof(uintptr_t));
		for (size_t j = 0; j < *places[i].count; ++j) {
			(*places[i].place)[j] =
				strtoll(it + 1, &it, places[i].base);
		}
	}
	return errno;
}

/** Set everything to zero */
void instruction_clean(instruction_t *insn)
{
	assert(insn);
	free(insn->src_reg);
	free(insn->dst_reg);
	free(insn->src_mem);
	free(insn->dst_mem);
	memset(insn, 0, sizeof(*insn));
}
