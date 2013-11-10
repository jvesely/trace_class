
#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include "instruction.h"
#include "address_table.h"

typedef struct {
	address_table_t atable;
	size_t reads;
	size_t writes;
	size_t total;
	size_t move;
	size_t compute;
	size_t ints;
	size_t floats;
} processor_t;


int processor_init(processor_t *processor);
void processor_clear(processor_t *processor);
void processor_fini(processor_t *processor);
int processor_add_instruction(processor_t *processor, const instruction_t *insn);



#endif
