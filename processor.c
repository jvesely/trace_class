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
#include <string.h>
#include <errno.h>
#include "processor.h"

/** Set everything to zero and allocate memory. */
int processor_init(processor_t *processor)
{
	if (!processor)
		return -ENOMEM;
	memset(processor, 0, sizeof(*processor));
	return address_table_init(&processor->atable);
}

/* Set everything to zero. */
void processor_clear(processor_t *processor)
{
	assert(processor);
	address_table_t backup = processor->atable;
	address_table_clear(&backup);
	memset(processor, 0, sizeof(*processor));
	processor->atable = backup;
}

/** Free allocated memory */
void processor_fini(processor_t *processor)
{
	assert(processor);
	address_table_fini(&processor->atable);
}

/** Process statistical information for one instruction. */
int processor_add_instruction(processor_t *processor, const instruction_t *insn)
{
	assert(processor);
	if (!insn)
		return -EINVAL;
	++processor->total;
	unsigned reads = 0, writes = 0;
	for (unsigned i = 0; i < insn->src_count; ++i)
	{
		address_table_record_read(&processor->atable, insn->src_mem[i]);
		reads = 1;
	}
	for (unsigned i = 0; i < insn->dst_count; ++i)
	{
		address_table_record_write(&processor->atable, insn->dst_mem[i]);
		writes = 1;
	}
	processor->reads += reads;
	processor->writes += writes;
	processor->compute +=
		insn->src_count == 0 && insn->dst_count == 0 ? 1 : 0;
	processor->move +=
		insn->src_count == insn->dst_reg_count || insn->dst_count == insn->src_reg_count ? 1 : 0;
	return 0;
}
