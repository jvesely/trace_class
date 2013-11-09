#include <assert.h>
#include <string.h>
#include <errno.h>
#include "processor.h"


int processor_init(processor_t *processor)
{
	assert(processor);
	memset(processor, 0, sizeof(*processor));
	return 0;
}

int processor_add_instruction(processor_t *processor, const instruction_t *insn)
{
	assert(processor);
	if (!insn)
		return -EINVAL;
	++processor->total;
	processor->reads += insn->src_count > 0 ? 1 : 0;
	processor->writes += insn->dst_count > 0 ? 1 : 0;
	processor->compute +=
		insn->src_count == 0 && insn->dst_count == 0 ? 1 : 0;
	processor->move +=
		insn->src_count == insn->dst_reg_count || insn->dst_count == insn->src_reg_count ? 1 : 0;
	return 0;
}
