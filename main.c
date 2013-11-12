
#include "instruction.h"
#include "processor.h"
#include "feature_vector.h"

#include <stdio.h>
#include <getopt.h>

#include <zlib.h>

enum {
	COUNT_LIMIT = 25000000,
};

typedef int (*vector_callback_t)(const char *, feature_vector_t *, void *);

static char *gz_read(void *file, char *buffer, size_t size)
{
	return gzgets(file, buffer, size);
}

static void process(const char *class, gzFile trace, vector_callback_t cb,
	void* arg)
{
	size_t count = 0;

	instruction_t i = {0};
	processor_t proc;
	processor_init(&proc);

	while (!instruction_get(&i, trace, gz_read)) {
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
	(void)arg;
	static int done = 0;
	if (!done) {
		printf("%s\n", class);
		done =1;
	}
	feature_vector_print(v);
	return 0;
}

struct option options[] = {
	{ "class", required_argument, NULL, 'c'},
	{ "file", required_argument, NULL, 'f'},
	{ "help", no_argument, NULL, 'h'},
};

static void help(const char *name)
{
	printf("%s usage\n"
		"\t--class,-c	The provided trace is of this class\n"
		"\t--file,-f	Use this file a a trace (stdin by default)\n"
		"\t--help,-h	This help\n", name);
}

int main(int argc, char **argv)
{
	const char * class = NULL;
	const char * file = NULL;
	gzFile trace = NULL;

	int c;
	while ((c = getopt(argc, argv, "f:c:h")) != -1) {
		switch (c) {
		case 'c': class = optarg; break;
		case 'f': file = optarg; break;
		default:
			help(argv[0]);
			return 1;
		}
	}
	if (file)
		trace = gzopen(file, "r");
	else
		trace = gzdopen(0, "r");
	if (!trace) {
		fprintf(stderr, "Failed to open input trace file\n");
		return 1;
	}
	process(class, trace, vector_print, NULL);

	gzclose(trace);

	return 0;
}
