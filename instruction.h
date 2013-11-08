
#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include <stdint.h> //uintptr_t
#include <stdio.h>  //FILE*
#include <errno.h>


typedef struct {
	unsigned code;
	unsigned category;
	unsigned src_reg_count;
	unsigned dst_reg_count;
	unsigned src_count;
	unsigned dst_count;
	uintptr_t *src_reg;
	uintptr_t *dst_reg;
	uintptr_t *src_mem;
	uintptr_t *dst_mem;
} instruction_t;


int instruction_get(instruction_t *insn, FILE* file);
void instruction_clean(instruction_t *insn);


#endif
