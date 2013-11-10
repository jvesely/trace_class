
#include "instruction.h"
#include "processor.h"
#include "feature_vector.h"

#include <stdio.h>

#define COUNT_LIMIT 10000000


int main(int argc, char **argv)
{
	FILE *trace = stdin;
	size_t count = 0;

	instruction_t i = {0};
	processor_t proc;
	processor_init(&proc);

	while (!instruction_get(&i, trace)) {
		++count;
		processor_add_instruction(&proc, &i);	
		if (count % COUNT_LIMIT == 0) {
			feature_vector_t v;
			feature_vector_init(&v, &proc);
			feature_vector_print(&v);
		}
		instruction_clean(&i);
	}
	feature_vector_t v;
	feature_vector_init(&v, &proc);
	feature_vector_print(&v);
	processor_fini(&proc);

	printf("Parsed instructions: %zu\n", count);

	return 0;
}
