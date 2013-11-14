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

#include "feature_vector.h"
#include "instruction.h"
#include "processor.h"
#include "storage.h"

#include <stdio.h>
#include <getopt.h>

#include <zlib.h>

enum {
	/** Default number of instructions per data point */
	COUNT_LIMIT = 25000000,
};

typedef int (*vector_callback_t)(const char *, feature_vector_t *, void *);

/** Read one line from gzip compressed file */
static char *gz_read(void *file, char *buffer, size_t size)
{
	return gzgets(file, buffer, size);
}

/** Read instructions from the provided trace and call %cb after %p
 * limit instructions has been parsed.
 */
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

/** Print the vector or store it in the database. Based on %p arg value. */
static int vector_print_store(const char *class, feature_vector_t *v, void *arg)
{
	if (arg) {
		storage_store_vector(arg, class, v);
	} else {
		feature_vector_print(v, class);
	}
	return 0;
}

/** supported cmdline options */
static struct option options[] = {
	{ "class", required_argument, NULL, 'c'},
	{ "file", required_argument, NULL, 'f'},
	{ "size", required_argument, NULL, 's'},
	{ "database", required_argument, NULL, 'd'},
	{ "help", no_argument, NULL, 'h'},
};

/** Help function prints usage help */
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
	/* Initialize defaults */
	const char * class = NULL;
	const char * file = NULL;
	const char * dbfile = NULL;
	gzFile trace = NULL;
	size_t limit = COUNT_LIMIT;

	/* Parse cmdline arguments */
	int c;
	while ((c = getopt_long(argc, argv, "f:c:s:d:h", options, NULL)) != -1)
	{
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


	/* Initialize sqlite3 storage engine */
	storage_t store;
	if (storage_init(&store, dbfile, limit)) {
		fprintf(stderr, "Failed to initialize DB storage: %s.\n"
			"Use -h to get usage help.\n", dbfile);
		return 1;
	}

	/* Open stdin if no file was provided */
	if (file) {
		trace = gzopen(file, "r");
	} else {
		trace = gzdopen(0, "r");
	}

	if (!trace) {
		fprintf(stderr, "Failed to open input trace file\n");
		return 1;
	}

	/* Start processing */
	process(class, trace, vector_print_store, &store, limit);

	/* Cleanup */
	storage_fini(&store);
	gzclose(trace);

	return 0;
}
