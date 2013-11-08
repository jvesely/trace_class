
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

int instruction_get(instruction_t *insn, FILE* file)
{
	if (!insn)
		return -ENOMEM;
	memset(insn, 0, sizeof(*insn));
	char tmp_buffer[1024];
	if (!fgets(tmp_buffer, 1023, file))
		return -1;

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
	for (unsigned i = 0; i < sizeof(places)/sizeof(places[0]); ++i) {
		*places[i].count = strtoll(it + 1, &it, 10);
		*places[i].place = calloc(*places[i].count, sizeof(uintptr_t));
		for (size_t j = 0; j < *places[i].count; ++j) {
			(*places[i].place)[j] = strtoll(it + 1, &it, places[i].base);
		}
	}
	return 0;
}

void instruction_clean(instruction_t *insn)
{
	assert(insn);
	free(insn->src_reg);
	free(insn->dst_reg);
	free(insn->src_mem);
	free(insn->dst_mem);
	memset(insn, 0, sizeof(*insn));
}
