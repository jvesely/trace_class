#include <assert.h>
#include <string.h>
#include <errno.h>
#include "processor.h"


int processor_init(processor_t *processor)
{
	if (!processor)
		return -ENOMEM;
	memset(processor, 0, sizeof(*processor));
	return address_table_init(&processor->atable);
}

void processor_clear(processor_t *processor)
{
	assert(processor);
	address_table_t backup = processor->atable;
	address_table_clear(&backup);
	memset(processor, 0, sizeof(*processor));
	processor->atable = backup;
}

void processor_fini(processor_t *processor)
{
	assert(processor);
	address_table_fini(&processor->atable);
}

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
