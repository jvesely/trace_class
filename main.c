
#include "feature_vector.h"
#include "instruction.h"
#include "processor.h"
#include "storage.h"

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
	void* arg, size_t limit)
{
	size_t count = 0;

	instruction_t i = {0};
	processor_t proc;
	processor_init(&proc);

	while (!instruction_get(&i, trace, gz_read)) {
		++count;
		processor_add_instruction(&proc, &i);	
		if (limit && (count % limit == 0)) {
			feature_vector_t v;
			feature_vector_init(&v, &proc);
			cb(class, &v, arg);
			processor_clear(&proc);
		}
		instruction_clean(&i);
	}
	feature_vector_t v;
	feature_vector_init(&v, &proc);
	cb(class, &v, arg);
	processor_fini(&proc);

	printf("Parsed instructions: %zu\n", count);
}

static int vector_print_store(const char *class, feature_vector_t *v, void *arg)
{
	if (arg) {
		storage_store_vector(arg, class, v);
	} else {
		feature_vector_print(v, class);
	}
	return 0;
}

static struct option options[] = {
	{ "class", required_argument, NULL, 'c'},
	{ "file", required_argument, NULL, 'f'},
	{ "size", required_argument, NULL, 's'},
	{ "database", required_argument, NULL, 'd'},
	{ "help", no_argument, NULL, 'h'},
};

static void help(const char *name)
{
	printf("%s usage\n"
		"\t--class,-c <arg>   \tThe provided trace is of this class\n"
		"\t--file,-f <arg>    \tUse this file as a trace (stdin by default)\n"
		"\t--size,-s <arg>    \tUse <arg> instructions for one data point\n"
		"\t                   \t0 means no limit. Default is %zu.\n"
		"\t--database,-d <arg>\tUse this file as a knowledge store backend\n"
		"\t--help,-h          \tThis help\n",
		name, (size_t)COUNT_LIMIT);
}

int main(int argc, char **argv)
{
	const char * class = NULL;
	const char * file = NULL;
	const char * dbfile = NULL;
	gzFile trace = NULL;
	size_t limit = COUNT_LIMIT;

	int c;
	while ((c = getopt_long(argc, argv, "f:c:s:d:h", options, NULL)) != -1) {
		switch (c) {
		case 'c': class = optarg; break;
		case 'f': file = optarg; break;
		case 'd': dbfile = optarg; break;
		case 's': sscanf(optarg, "%zu", &limit); break;
		default:
			help(argv[0]);
			return 1;
		}
	}

	storage_t store;
	storage_init(&store, dbfile, limit);

	if (file)
		trace = gzopen(file, "r");
	else
		trace = gzdopen(0, "r");
	if (!trace) {
		fprintf(stderr, "Failed to open input trace file\n");
		return 1;
	}
	process(class, trace, vector_print_store, &store, limit);

	storage_fini(&store);
	gzclose(trace);

	return 0;
}
