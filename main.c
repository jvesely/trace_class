
#include "instruction.h"
#include "processor.h"
#include "feature_vector.h"

#include <stdio.h>

enum {
	COUNT_LIMIT = 25000000,
};

typedef int (*vector_callback_t)(const char *, feature_vector_t *, void *);

static void process(const char *class, FILE *trace, vector_callback_t cb,
	void* arg)
{
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
			cb(class, &v, arg);
			processor_clear(&proc);
		}
		instruction_clean(&i);
	}
	feature_vector_t v;
	feature_vector_init(&v, &proc);
	feature_vector_print(&v);
	processor_fini(&proc);

	printf("Parsed instructions: %zu\n", count);
}

static int vector_print(const char *class, feature_vector_t *v, void *arg)
{
	(void)class;
	(void)arg;
	feature_vector_print(v);
	return 0;
}
int main(int argc, char **argv)
{
	process("no class", stdin, vector_print, NULL);

	return 0;
}
